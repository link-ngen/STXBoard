/*
 * netx_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include <stdio.h>

#include "app_config.h"
#include "app_manager.h"
#include "netx_worker.h"
#include "SerialDPMInterface.h"

extern NETX_COMM_CHANNEL_HANDLER_T g_tRealtimeEthernetHandler;
extern NETX_COMM_CHANNEL_HANDLER_T g_tNetworkServicesHandlers;

static NetxRessource_t tNetxFSM = { 0 };

static void State_NetxInit(NetxRessource_t *ptNetxRsc);
static void State_NetxPreOP(NetxRessource_t *ptNetxRsc);
static void State_NetxOP(NetxRessource_t *ptNetxRsc);
static void State_NetxSTOP(NetxRessource_t *ptNetxRsc);
static void State_NetxError(NetxRessource_t *ptNetxRsc);

static bool isCookieAvailable(PDEVICEINSTANCE ptDevInstance, uint32_t ulTimeoutInMs, NetxRessource_t *ptNetxRsc)
{
  bool fCookieAvailable = false;
  char szCookie[5] = { 0 };
  uint32_t starttime, difftime = 0;
  LedTaskCommand_t tLedCmd = LED_STATUS_UNKNOWN;

  starttime = OS_GetMilliSecCounter();

  while(false == fCookieAvailable && difftime < ulTimeoutInMs)
  {
    OS_Memset(szCookie, 0, sizeof(szCookie));

    HWIF_READN(ptDevInstance, szCookie, ptDevInstance->pbDPM, 4);

    /** on DPM cards we need to check the for a valid cookie */
    if((0 == OS_Strcmp(szCookie, CIFX_DPMSIGNATURE_BSL_STR)) ||
      (0 == OS_Strcmp(szCookie, CIFX_DPMSIGNATURE_FW_STR)))
    {
      /** We have a firmware or bootloader running, so we assume it is a flash based device */
      /** NOTE: If the driver is restarted and a RAM based FW was downloaded before this
       will result in the device being handled as flash based.
       Currently there is no way to detect this */
      fCookieAvailable = true;
      tLedCmd = LED_STATUS_ERROR_OFF;
    }
    else
    {
      fCookieAvailable = false;
      difftime = OS_GetMilliSecCounter() - starttime;

      tLedCmd = LED_STATUS_ERROR_ON;
    }
    xQueueSend(ptNetxRsc->tAppQueues->ledQueue, &tLedCmd, 10);
  }

  if(false == fCookieAvailable)
  {
    //printf("DPM cookie not available since %u milliseconds\r\n", (unsigned int) ulTimeoutInMs);
  }

  return fCookieAvailable;
}

static int32_t InitializeToolkit(NetxRessource_t *ptNetxRsc)
{
  int32_t lRet = cifXTKitInit();

  if(CIFX_NO_ERROR == lRet)
  {
    PDEVICEINSTANCE ptDevInstance = (PDEVICEINSTANCE) OS_Memalloc(sizeof(*ptDevInstance));
    OS_Memset(ptDevInstance, 0, sizeof(*ptDevInstance));

    /** Set trace level of toolkit */
    g_ulTraceLevel = TRACE_LEVEL_ERROR |
    TRACE_LEVEL_WARNING |
    TRACE_LEVEL_INFO |
    TRACE_LEVEL_DEBUG;

    /** Insert the basic device information into the DeviceInstance structure
     for the toolkit.
     NOTE: The irq number are for information use only, so we skip them here.
     Interrupt is currently not supported and ignored, so we don't need to set it */
    ptDevInstance->fPCICard = 0;
    ptDevInstance->pvOSDependent = ptDevInstance;
    ptDevInstance->ulDPMSize = 0x8000;   // 32K
    ptDevInstance->ulIrqNumber = 7;
    ptDevInstance->eDeviceType = eCIFX_DEVICE_FLASH_BASED;
    OS_Strncpy(ptDevInstance->szName, "cifX0", sizeof(ptDevInstance->szName));

    /** we know here that netX firmware is flash based, therefore we check if it starts up
     ** by comparing cookie at DPM address 0x00 is valid.*/
    do
    {
      SerialDPM_Init(ptDevInstance);
    } while(false == isCookieAvailable(ptDevInstance, 100, ptNetxRsc));

    /* Add the device to the toolkits handled device list */
    lRet = cifXTKitAddDevice(ptDevInstance);

    /** If it succeeded do device tests */
    if(CIFX_NO_ERROR != lRet)
    {
      /** Uninitialize Toolkit, this will remove all handled boards from the toolkit and
       deallocate the device instance */
      cifXTKitDeinit();
    }
  }

  return lRet;
}

/**************************************************************************************/
/*! Call the event handler function for all channels
*
*   \param ptAppData    [in]  Pointer to application data
*   \param szBoardName  [in]  Name of the board that shall be accessed
*
*   \return 0 on success.
*/
/**************************************************************************************/
static int32_t NetxInitializeChannels(NetxRessource_t *ptNetxData, char *szBoardName)
{
  int32_t lRet = CIFX_NO_ERROR;
  for(uint8_t i = 0; i < USED_COMMUNICATION_CHANNELS; ++i)
  {
    NETX_COMM_CHANNEL_T *ptCommChannel = &ptNetxData->atCommChannels[i];
    CHANNEL_INFORMATION *ptChannelInfo = &ptCommChannel->tCifXChannelInfo;
    uint32_t ulState = 0;

    if(ptCommChannel->tCommChannelHandler.pfnInitialize != NULL)
    {
      /* Open the driver handle of the channel */
      lRet = xChannelOpen(ptNetxData->hDriver, szBoardName, i, &ptCommChannel->hCifXChannel);
      if(CIFX_NO_ERROR != lRet)
      {
        PRINTF("ERROR: xChannelOpen for %s, channel %d failed: 0x%08X" NEWLINE, szBoardName, i, (unsigned int)lRet);
        return lRet;
      }

      /* Wait until the channel is ready (If the channel is not ready we can't send packets) */
      do
      {
        memset(ptChannelInfo, 0, sizeof(*ptChannelInfo));

        /** Retrieve the global communication channel information */
        if(CIFX_NO_ERROR != (lRet = xChannelInfo(ptCommChannel->hCifXChannel, sizeof(CHANNEL_INFORMATION), ptChannelInfo)))
        {
          PRINTF("ERROR: Querying communication channel information block: 0x%08X !" NEWLINE, (unsigned int) lRet);
          return lRet;
        }
        else
        {
          PRINTF("Communication Channel Info:" NEWLINE);
          PRINTF("Device Number   : %u" NEWLINE, (unsigned int) ptChannelInfo->ulDeviceNumber);
          PRINTF("Serial Number   : %u" NEWLINE, (unsigned int) ptChannelInfo->ulSerialNumber);
          PRINTF("Firmware        : %s" NEWLINE, ptChannelInfo->abFWName);
          PRINTF("FW Version      : %u.%u.%u.%u" NEWLINE,
                (unsigned int) ptChannelInfo->usFWMajor,
                (unsigned int) ptChannelInfo->usFWMinor,
                (unsigned int) ptChannelInfo->usFWBuild,
                (unsigned int) ptChannelInfo->usFWRevision);

          PRINTF("FW Date         : %02u/%02u/%04u" NEWLINE,
            (unsigned int) ptChannelInfo->bFWMonth,
            (unsigned int) ptChannelInfo->bFWDay,
            (unsigned int) ptChannelInfo->usFWYear);
          PRINTF("Mailbox Size    : %u" NEWLINE,
            (unsigned int) ptChannelInfo->ulMailboxSize);
        }
      } while(!(ptChannelInfo->ulDeviceCOSFlags & HIL_COMM_COS_READY)
        || (ptChannelInfo->ulDeviceCOSFlags == CIFX_DPM_NO_MEMORY_ASSIGNED));

      /** Set the Application state flag in the application COS flags */
      if(CIFX_NO_ERROR != (lRet = xChannelHostState(ptCommChannel->hCifXChannel, CIFX_HOST_STATE_READY, &ulState, 5)))
      {
        PRINTF("ERROR: xChannelHostState failed: 0x%08X" NEWLINE, (unsigned int)lRet);
        return lRet;
      }

      /** Initialize the Stack hander */
      if(CIFX_NO_ERROR
        != (lRet = ptCommChannel->tCommChannelHandler.pfnInitialize(&ptCommChannel->hCommChannelRsc, ptCommChannel->hCifXChannel, ptChannelInfo)))
      {
        PRINTF("ERROR: Channel initialize failed: 0x%08X" NEWLINE, (unsigned int)lRet);
        return lRet;
      }
    }
  }
  return CIFX_NO_ERROR;
}

/*! Call the start configuration function for all channels
*
*   \param ptAppData      [in]  Pointer to application data
*
*   \return 0 on success.
*/
static int32_t NetX_ConfigureChannels(NetxRessource_t* ptNetxData)
{
  int32_t lRet = CIFX_NO_ERROR;

  for(int i = 0; i < USED_COMMUNICATION_CHANNELS; i++)
  {
    NETX_COMM_CHANNEL_T *ptCommChannel = &ptNetxData->atCommChannels[i];
    if(NULL != ptCommChannel->tCommChannelHandler.pfnSetup)
    {
      lRet = ptCommChannel->tCommChannelHandler.pfnSetup(ptCommChannel->hCommChannelRsc);
      if(CIFX_NO_ERROR != lRet)
      {
        PRINTF("ERROR: Channel setup failed: 0x%08X" NEWLINE, (unsigned int)lRet);
        return lRet;
      }

    }
  }
  return CIFX_NO_ERROR;
}

/**************************************************************************************/
/*! Close all opened channels
*
*   \param ptAppData    [in]  Pointer to application data
*
*   \return 0 on success.
*/
/**************************************************************************************/
static void NetX_AllChannels_Close(NetxRessource_t* ptNetxData)
{
  for (uint8_t i = 0; i < USED_COMMUNICATION_CHANNELS; ++i)
  {
    NETX_COMM_CHANNEL_T *ptCommChannel =  &ptNetxData->atCommChannels[i];
    if (ptCommChannel->hCifXChannel != NULL)
    {
      ptCommChannel->tCommChannelHandler.pfnDeInitialize(ptNetxData->atCommChannels[i].hCommChannelRsc, ptCommChannel->hCifXChannel);
      xChannelClose(ptCommChannel->hCifXChannel);
      ptCommChannel->hCifXChannel = NULL;
    }
  }
}

void State_NetxInit(NetxRessource_t *ptNetxRsc)
{
  int32_t lRet = InitializeToolkit(ptNetxRsc);
  PRINTF("---------- STX Application ----------" NEWLINE);

  LedTaskCommand_t tLedCmd = LED_STATUS_UNKNOWN;
  LcdPacket_T tLcdPacket = { LCD_COMMAND_UNKNOWN, "init...\n" };

  if (CIFX_NO_ERROR == lRet)
  {
    ptNetxRsc->atCommChannels[0].tCommChannelHandler = g_tRealtimeEthernetHandler;
#if USED_COMMUNICATION_CHANNELS >= 2
    ptNetxRsc->atCommChannels[1].tCommChannelHandler = g_tNetworkServicesHandlers;
#endif
    lRet = xDriverOpen(&ptNetxRsc->hDriver);
    if (CIFX_NO_ERROR != lRet)
    {
      ptNetxRsc->currentState = State_NetxError;
      tLedCmd = LED_STATUS_ERROR_ON;
      tLcdPacket.tCmd = LCD_COMMAND_ERROR_SCREEN;
      tLcdPacket.pcMessage = "DrvOpen err \n";
    }
    else
    {
      ptNetxRsc->currentState = State_NetxPreOP;
      tLedCmd = LED_STATUS_CONFIG_BLINK;
      tLcdPacket.tCmd = LCD_COMMAND_BOOT_SCREEN;
      tLcdPacket.pcMessage = "DrvOpen ok \n";
    }
  }
  else
  {
    ptNetxRsc->currentState = State_NetxError;
    tLedCmd = LED_STATUS_ERROR_ON;
    tLcdPacket.tCmd = LCD_COMMAND_IDLE_SCREEN;
  }
  ptNetxRsc->lastState = State_NetxInit;

  xQueueSend(ptNetxRsc->tAppQueues->ledQueue, &tLedCmd, 10);
  xQueueSend(ptNetxRsc->tAppQueues->lcdQueue, &tLcdPacket, 10);
}

void State_NetxPreOP(NetxRessource_t *ptNetxRsc)
{
  int32_t lRet = CIFX_NO_ERROR;
  LedTaskCommand_t tLedCmd = LED_STATUS_CONFIG_BLINK;
  LcdPacket_T tLcdPacket = { LCD_COMMAND_CONFIG_SCREEN, "configuring...\n" };

  if (CIFX_NO_ERROR != (lRet = NetxInitializeChannels(ptNetxRsc, "cifX0")))
  {
    ptNetxRsc->currentState = State_NetxError;
    tLedCmd = LED_STATUS_ERROR_ON;
    tLcdPacket.tCmd = LCD_COMMAND_ERROR_SCREEN;
    tLcdPacket.pcMessage = "ChnOpen err \n";
  }
  else if (CIFX_NO_ERROR != (lRet = NetX_ConfigureChannels(ptNetxRsc)))
  {
    ptNetxRsc->currentState = State_NetxError;
    tLedCmd = LED_STATUS_ERROR_ON;
    tLcdPacket.tCmd = LCD_COMMAND_ERROR_SCREEN;
    tLcdPacket.pcMessage = "ChnCfg err \n";
  }
  else
  {
    /* config done */
    ptNetxRsc->fNetXDrvRunning = true;
    ptNetxRsc->lastState = State_NetxOP;

    tLedCmd = LED_STATUS_RUN_ON;
    tLcdPacket.tCmd = LCD_COMMAND_VERTEX_SCREEN;
    tLcdPacket.pcMessage = "";
  }

  xQueueSend(ptNetxRsc->tAppQueues->ledQueue, &tLedCmd, 10);
  xQueueSend(ptNetxRsc->tAppQueues->lcdQueue, &tLcdPacket, 10);
}

void State_NetxOP(NetxRessource_t *ptNetxRsc)
{
  /* TODO: prepare the data to io struct object */
  if (NULL != ptNetxRsc->atCommChannels[0].tCommChannelHandler.pfnCyclicTask)
  {
    ptNetxRsc->atCommChannels[0].tCommChannelHandler.pfnCyclicTask(ptNetxRsc->atCommChannels[0].hCommChannelRsc);
  }
  /* TODO: Check communication state. */
  ptNetxRsc->lastState = State_NetxOP;
  OS_Sleep(1);
}

void State_NetxError(NetxRessource_t *ptNetxRsc)
{
//  LedTaskCommand_t tLedCmd = LED_STATUS_ERROR_ON;
//  LcdPacket_T tLcdPacket = { LCD_COMMAND_ERROR_SCREEN, "Error\n" }; /* Error screen */
//
//  xQueueSend(ptNetxRsc->tAppQueues->ledQueue, &tLedCmd, 10);
//  xQueueSend(ptNetxRsc->tAppQueues->lcdQueue, &tLcdPacket, 10);

  if (ptNetxRsc->fNetXDrvRunning)
  {
    ptNetxRsc->fNetXDrvRunning = false;
    //CloseNetxChannels(ptNetxRsc);
  }

  if (ptNetxRsc->lastState == State_NetxInit)
  {
    /* TODO: DeInitialize Toolkit and free memory */
    NetX_AllChannels_Close(ptNetxRsc);
    cifXTKitDeinit();

    ptNetxRsc->currentState = State_NetxInit;
  }
  else
  {
    /*TODO: reconfig real time ethernet ? */
    ptNetxRsc->currentState = State_NetxPreOP;
  }
  ptNetxRsc->lastState = State_NetxError;
}

/**
 * Main state machine logic, runs the current state function
 */
static void fsm_run(NetxRessource_t *ptNetxRsc)
{
  ptNetxRsc->currentState(ptNetxRsc);
}

void NetxWorker(void *pvParameters)
{
  tNetxFSM.currentState = State_NetxInit;
  tNetxFSM.lastState = NULL;
  tNetxFSM.tAppQueues = (AppQueues_t *)pvParameters;

  while (1)
  {
    fsm_run(&tNetxFSM);
  }
}

