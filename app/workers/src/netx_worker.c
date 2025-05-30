/*
 * netx_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include <stdio.h>

#include "app_manager.h"
#include "netx_worker.h"
#include "SerialDPMInterface.h"

static NetxFsmRessource_t tNetxFSM = { 0 };

static void State_NetxInit(NetxFsmRessource_t *ptNetxRsc);
static void State_NetxPreOP(NetxFsmRessource_t *ptNetxRsc);
static void State_NetxOP(NetxFsmRessource_t *ptNetxRsc);
static void State_NetxError(NetxFsmRessource_t *ptNetxRsc);

static bool isCookieAvailable(PDEVICEINSTANCE ptDevInstance, uint32_t ulTimeoutInMs, NetxFsmRessource_t *ptNetxRsc)
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

static int32_t InitializeToolkit(NetxFsmRessource_t *ptNetxRsc)
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

static void CloseNetxChannels(NetxFsmRessource_t *ptNetxRsc)
{
  if(ptNetxRsc->tChannelRsc.hChannel)
  {
    xChannelClose(ptNetxRsc->tChannelRsc.hChannel);
  }

  if(ptNetxRsc->tSysRsc.hSys)
  {
    xSysdeviceClose(ptNetxRsc->tSysRsc.hSys);
  }
}

void State_NetxInit(NetxFsmRessource_t *ptNetxRsc)
{
  int32_t lRet = InitializeToolkit(ptNetxRsc);

  LedTaskCommand_t tLedCmd = LED_STATUS_UNKNOWN;
  LcdPacket_T tLcdPacket = { LCD_COMMAND_UNKNOWN, "init...\n" };

  if (CIFX_NO_ERROR == lRet)
  {
    ptNetxRsc->currentState = State_NetxPreOP;
    tLedCmd = LED_STATUS_RUN_ON;
    tLcdPacket.tCmd = LCD_COMMAND_BOOT_SCREEN;
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

void State_NetxPreOP(NetxFsmRessource_t *ptNetxRsc)
{
  LedTaskCommand_t tLedCmd = LED_STATUS_CONFIG_BLINK;
  LcdPacket_T tLcdPacket = { LCD_COMMAND_CONFIG_SCREEN, "configuring...\n" };

  xQueueSend(ptNetxRsc->tAppQueues->ledQueue, &tLedCmd, 10);
  xQueueSend(ptNetxRsc->tAppQueues->lcdQueue, &tLcdPacket, 10);

  /* TODO: Real Time Ethernet configuration */
  vTaskDelay(10000);

  /* config done */
  //ptNetxRsc->fNetXDrvRunning = true;

  ptNetxRsc->lastState = State_NetxError;
}

void State_NetxOP(NetxFsmRessource_t *ptNetxRsc)
{
  LedTaskCommand_t tLedCmd = LED_STATUS_RUN_ON;
  xQueueSend(ptNetxRsc->tAppQueues->ledQueue, &tLedCmd, 10);

  /* TODO: implement cyclic data transfer */
  ptNetxRsc->lastState = State_NetxOP;
}

void State_NetxError(NetxFsmRessource_t *ptNetxRsc)
{
  LedTaskCommand_t tLedCmd = LED_STATUS_ERROR_ON;
  LcdPacket_T tLcdPacket = { LCD_COMMAND_ERROR_SCREEN, "Error\n" }; /* Error screen */

  xQueueSend(ptNetxRsc->tAppQueues->ledQueue, &tLedCmd, 10);
  xQueueSend(ptNetxRsc->tAppQueues->lcdQueue, &tLcdPacket, 10);

  if (ptNetxRsc->fNetXDrvRunning)
  {
    ptNetxRsc->fNetXDrvRunning = false;
    CloseNetxChannels(ptNetxRsc);
  }
  if (ptNetxRsc->lastState == State_NetxInit)
  {
    ptNetxRsc->currentState = ptNetxRsc->lastState;
  }
  else
  {
    ptNetxRsc->currentState = State_NetxPreOP;
  }
  ptNetxRsc->lastState = State_NetxError;
}

/**
 * Main state machine logic, runs the current state function
 */
static void fsm_run(NetxFsmRessource_t *ptNetxRsc)
{
  ptNetxRsc->currentState(ptNetxRsc);
}

void NetxDemoWorker(void *pvParameters)
{
  tNetxFSM.currentState = State_NetxInit;
  tNetxFSM.lastState = NULL;
  tNetxFSM.tAppQueues = (AppQueues_t *)pvParameters;

  while (1)
  {
    fsm_run(&tNetxFSM);
  }
}
