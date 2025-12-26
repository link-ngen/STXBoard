/*
 * pns_app.c
 *
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#include "pns_functions.h"
#include "pns_config.h"
#include "cifXErrors.h"
#include "cifXUser.h"
#include "app_config.h"
#include "SystemPackets.h"

static int PNS_Initialize(NETX_PROTOCOL_RSC_T *ptProtocolRsc, CIFXHANDLE hCifXChannel, CHANNEL_INFORMATION *ptCifXChannelInfo )
{
  uint32_t ulRet = CIFX_NO_ERROR;
  ptProtocolRsc->hCifXChannel = hCifXChannel;
  ptProtocolRsc->tCifXChannelInfo = *ptCifXChannelInfo;
  ptProtocolRsc->fDeviceIsRunning = false;

  if(Pkt_Init(&(ptProtocolRsc->hPktIfRsc), ptProtocolRsc->hCifXChannel))
  {
    Pkt_RegisterPacketHandler(ptProtocolRsc->hPktIfRsc, PNS_PacketHandler, ptProtocolRsc);
  }

#ifdef HOST_APPLICATION_SETS_MAC_ADDRESS
  /* optional when initial DDP state is passive: set DDP base device parameters: MAC address */
  ulRet = AppPNS_SetMacAddress( ptRsc );
  if( ulRet != CIFX_NO_ERROR )
    return (int)ulRet;
#endif

#ifdef HOST_APPLICATION_SETS_SERIAL_NUMBER
  /* optional when initial DDP state is passive: set additional (OEM) DDP base device parameters: serial number */
  ulRet = AppPNS_SetOemData( ptRsc );
  if(ulRet != CIFX_NO_ERROR)
    return (int)ulRet;
#endif

#if defined(HOST_APPLICATION_SETS_MAC_ADDRESS) || defined(HOST_APPLICATION_SETS_SERIAL_NUMBER)
  ulRet = AppPNS_ActivateDdp( ptRsc );
  if(ulRet != CIFX_NO_ERROR)
    return (int)ulRet;


  /* According to netX Dual-Port Memory Packet-based services netx 100 page 65,
   * a state change from passive to active mode may require a certain time.
   * we wait ~100ms, just in case */
  OS_Sleep(100);
#endif

  return ulRet;
}

/**************************************************************************************
*   PNS Application Setup
*
*   \param hRsc
*
*   \return CIFX_NO_ERROR on success.
**************************************************************************************/
static int PNS_Setup(NETX_PROTOCOL_RSC_T *ptProtocolRsc )
{
  uint32_t lRet = CIFX_NO_ERROR;

  /* Download the configuration */
  lRet = PNS_ConfigureStack( ptProtocolRsc );
  if (CIFX_NO_ERROR == lRet)
  {
    ptProtocolRsc->fDeviceIsRunning = true;
  }

  return lRet;
}

static int PNS_IOTask(NETX_PROTOCOL_RSC_T *ptProtocolRsc)
{
  long lRet = CIFX_NO_ERROR; /** Return value for common error codes  */

  /** INPUT DATA *********************************************************************/
  lRet = xChannelIORead(ptProtocolRsc->hCifXChannel, 0, 0, sizeof(ptProtocolRsc->abActorData), ptProtocolRsc->abActorData, 0);
  if(CIFX_NO_ERROR != lRet)
  {
    /** Something failed?
     * Reason for error could be:
     * 1) netX is not "ready" yet. May happen during startup.
     * 2) netX is not "running" yet. May happen during startup in case the netX is not fully configured yet.
     * 3) netX has not yet established an IO connection. */
    return lRet;
  }

  /* Process data... */
  // TODO: process data in the same cycle?

  /** OUTPUT DATA ***************************************/
  lRet = xChannelIOWrite(ptProtocolRsc->hCifXChannel, 0, 0, sizeof(ptProtocolRsc->abSensorData), ptProtocolRsc->abSensorData, 0);
  return lRet;
}

/**************************************************************************************
*   De-Initialize PNS Application
*
*   \param phCommChHdlRsc
**************************************************************************************/
static void PNS_DeInitialize(NETX_PROTOCOL_RSC_T *ptProtocolRsc, CIFXHANDLE hCifXChannel)
{
  PRINTF("---------- Free memory and unregister callbacks ----------" NEWLINE);

  uint32_t ulRet = CIFX_NO_ERROR;
  uint32_t            ulState = 0;

  if (NULL == ptProtocolRsc) return;

  /* First, turn the bus off */
  SysPkt_AssembleStartStopCommReq(&ptProtocolRsc->tPacket, false);
  ulRet = Pkt_SendReceivePacket(ptProtocolRsc->hPktIfRsc, &ptProtocolRsc->tPacket, TXRX_TIMEOUT);

  if (CIFX_NO_ERROR != (ulRet = xChannelHostState(ptProtocolRsc->hCifXChannel, CIFX_HOST_STATE_NOT_READY, &ulState, 5)))
  {
      PRINTF("ERROR: xChannelHostState failed: 0x%08X" NEWLINE, (unsigned int)ulRet);
      return;
  }

  SysPkt_AssembleDeleteConfig(&ptProtocolRsc->tPacket);
  (void)Pkt_SendReceivePacket(ptProtocolRsc->hPktIfRsc, &ptProtocolRsc->tPacket, TXRX_TIMEOUT);

  PRINTF("---------- Cleaning finished ----------" NEWLINE);
}

static uint32_t PNS_ReadNetworkState(NETX_PROTOCOL_RSC_T *ptProtocolRsc)
{
  uint32_t lRet = CIFX_NO_ERROR;
  HIL_DPM_COMMON_STATUS_BLOCK_T tCommonStatusBlock = { 0 };
  lRet = xChannelCommonStatusBlock(ptProtocolRsc->hCifXChannel, CIFX_CMD_READ_DATA, 0, sizeof(tCommonStatusBlock), &tCommonStatusBlock);
  if (CIFX_NO_ERROR == lRet)
  {
    lRet = tCommonStatusBlock.ulCommunicationState;
  }
  return lRet;
}

void PNS_MailboxTask(NETX_PROTOCOL_RSC_T *ptProtocolRsc)
{
  uint32_t ulTXMbxCnt = 0, ulRXMbxCnt = 0;
  uint32_t ulRet = CIFX_NO_ERROR;

  while(1)
  {
    ulRet = xChannelGetMBXState(ptProtocolRsc->hCifXChannel, &ulRXMbxCnt, &ulTXMbxCnt);
    if(CIFX_NO_ERROR == ulRet && ulRXMbxCnt > 0)
    {
      Pkt_CheckReceiveMailbox(ptProtocolRsc->hPktIfRsc, &ptProtocolRsc->tPacket);
    }
    else
      break;
  }
}

NETX_PROTOCOL_DESC_T g_tRealtimeEthernetHandler =
{
  .pfnInitialize          = PNS_Initialize,
  .pfnSetup               = PNS_Setup,
  .pfnCyclicTask          = PNS_IOTask,
  .pfnMailboxTask         = PNS_MailboxTask,
  .pfnReadNetworkState    = PNS_ReadNetworkState,
  .pfnDeInitialize        = PNS_DeInitialize,
};
