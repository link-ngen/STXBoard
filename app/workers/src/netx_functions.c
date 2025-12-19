/*
 * netx_functions.c
 *
 *  Created on: 16.12.2025
 *      Author: nnguyen
 */

#include "netx_worker.h"
#include "SerialDPMInterface.h"
#include "app_config.h"

static PDEVICEINSTANCE ptDevInstance;

static bool isCookieAvailable(PDEVICEINSTANCE ptDevInstance, uint32_t ulTimeoutInMs, NETX_APP_RSC_T *ptNetxRsc)
{
  bool fCookieAvailable = false;
  char szCookie[5] = { 0 };
  uint32_t starttime, difftime = 0;

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
    }
    else
    {
      fCookieAvailable = false;
      difftime = OS_GetMilliSecCounter() - starttime;
    }
  }

  if(false == fCookieAvailable)
  {
    //printf("DPM cookie not available since %u milliseconds\r\n", (unsigned int) ulTimeoutInMs);
  }

  return fCookieAvailable;
}

int32_t InitializeToolkit(NETX_APP_RSC_T *ptNetxRsc)
{
  int32_t lRet = cifXTKitInit();

  if(CIFX_NO_ERROR == lRet)
  {
    ptDevInstance = (PDEVICEINSTANCE) OS_Memalloc(sizeof(*ptDevInstance));
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
int32_t NetX_InitializeChannels(NETX_APP_RSC_T *ptNetxRsc, char *szBoardName)
{
  int32_t lRet = CIFX_NO_ERROR;
  for(uint8_t i = 0; i < USED_COMMUNICATION_CHANNELS; ++i)
  {
    NETX_PROTOCOL_RSC_T *ptCommChannel = ptNetxRsc->atCommChannels[i];
    CHANNEL_INFORMATION *ptChannelInfo = &ptCommChannel->tCifXChannelInfo;
    uint32_t ulState = 0;

    if(ptCommChannel->tProtocolDesc.pfnInitialize != NULL)
    {
      /* Open the driver handle of the channel */
      lRet = xChannelOpen(ptNetxRsc->hDriver, szBoardName, i, &ptCommChannel->hCifXChannel);
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
      if(CIFX_NO_ERROR !=
        (lRet = ptCommChannel->tProtocolDesc.pfnInitialize(ptCommChannel, ptCommChannel->hCifXChannel, ptChannelInfo)))
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
*   \param ptNetxRsc      [in]  Pointer to application data
*
*   \return 0 on success.
*/
int32_t NetX_ConfigureChannels(NETX_APP_RSC_T* ptNetxRsc)
{
  int32_t lRet = CIFX_NO_ERROR;

  for(int i = 0; i < USED_COMMUNICATION_CHANNELS; i++)
  {
    NETX_PROTOCOL_RSC_T *ptCommChannel = ptNetxRsc->atCommChannels[i];
    if(NULL != ptCommChannel->tProtocolDesc.pfnSetup)
    {
      lRet = ptCommChannel->tProtocolDesc.pfnSetup(ptCommChannel);
      if(CIFX_NO_ERROR != lRet)
      {
        PRINTF("ERROR: Channel setup failed: 0x%08X" NEWLINE, (unsigned int)lRet);
        return lRet;
      }

    }
  }
  return CIFX_NO_ERROR;
}

/*! Call the packet handler function for all communication channel
*
*   \param ptNetxRsc      [in]  Pointer to application data
*
*/
void NetX_CallCommMailboxRoutine(NETX_APP_RSC_T* ptNetxRsc)
{
  NETX_PROTOCOL_RSC_T *ptCommChannel = ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL];

  if (ptCommChannel->tProtocolDesc.pfnMailboxTask != NULL)
  {
    ptCommChannel->tProtocolDesc.pfnMailboxTask(ptCommChannel);
  }
}

/**************************************************************************************/
/*! Close all opened channels
*
*   \param ptNetxRsc    [in]  Pointer to application data
*
*   \return 0 on success.
*/
/**************************************************************************************/
void NetX_AllChannels_Close(NETX_APP_RSC_T* ptNetxRsc)
{
  for (uint8_t i = 0; i < USED_COMMUNICATION_CHANNELS; ++i)
  {
    NETX_PROTOCOL_RSC_T *ptCommChannel = ptNetxRsc->atCommChannels[i];
    if (ptCommChannel->hCifXChannel != NULL)
    {
      ptCommChannel->tProtocolDesc.pfnDeInitialize(ptCommChannel, ptCommChannel->hCifXChannel);
      xChannelClose(ptCommChannel->hCifXChannel);
      ptCommChannel->hCifXChannel = NULL;
    }
  }
}

uint32_t Netx_ReadNetworkState(NETX_APP_RSC_T *ptNetxRsc)
{
  return ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->tProtocolDesc.pfnReadNetworkState(ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]);
}

int32_t NetX_ProcessIOData(NETX_APP_RSC_T *ptNetxRsc)
{
  int32_t lRet = CIFX_NO_ERROR;


  return lRet;
}
