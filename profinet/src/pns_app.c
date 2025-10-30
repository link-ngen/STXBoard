/*
 * pns_app.c
 *
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#include "NetxComChanIF.h"
#include "pns_functions.h"
#include "pns_config.h"
#include "cifXErrors.h"
#include "cifXUser.h"
#include "app_config.h"
#include "SystemPackets.h"

static PNS_RESSOURCES_T s_tPnsRsc;

static int PNS_Initialize(NETX_COMM_CHANNEL_HANDLER_RSC_H *phStackRsc, CIFXHANDLE hCifXChannel, CHANNEL_INFORMATION *ptCifXChannelInfo )
{
  uint32_t ulRet = CIFX_NO_ERROR;
  PNS_RESSOURCES_T *ptRsc = &s_tPnsRsc;
  ptRsc->hCifXChannel = hCifXChannel;
  ptRsc->ptCifXChannelInfo = ptCifXChannelInfo;

  if (Pkt_Init(&(ptRsc->hPktIfRsc), ptRsc->hCifXChannel))
  {
      Pkt_RegisterPacketHandler(ptRsc->hPktIfRsc, PNS_PacketHandler, ptRsc);
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

  *phStackRsc = (NETX_COMM_CHANNEL_HANDLER_RSC_H)ptRsc;

  return ulRet;
}

/**************************************************************************************
*   PNS Application Setup
*
*   \param hRsc
*
*   \return CIFX_NO_ERROR on success.
**************************************************************************************/
static int PNS_Setup(NETX_COMM_CHANNEL_HANDLER_RSC_H hRsc )
{
  PNS_RESSOURCES_T *ptRsc = (PNS_RESSOURCES_T*)hRsc;
  /* Download the configuration */
  return PNS_ConfigureStack( ptRsc );
}


/**************************************************************************************/
/*! Updates the application specific cyclic counter.
*  This function must be called on each IO data update cycle.
*  Note: this counter is implemented for demo purposes only.
*
* \param ptAppData  [in]  Pointer to application data
*/
/**************************************************************************************/
static void IO_UpdateCycleCounter( PNS_RESSOURCES_T* ptRsc )
{
  static uint32_t s_ulCycle = 0;

  s_ulCycle++;

  if (ptRsc->tInputData.bCyclicCounter_Speed > 0)
  {
    if (s_ulCycle >= ptRsc->tInputData.bCyclicCounter_Speed)
    {
      if (APP_CYCLE_COUNTER_COUNT_UP == ptRsc->tInputData.bCyclicCounter_Direction)
      {
        ptRsc->tOutputData.usCyclicCounter++;
      }
      else
      {
        ptRsc->tOutputData.usCyclicCounter--;
      }

      s_ulCycle = 0;
    }
  }
  else
  {
    ptRsc->tOutputData.usCyclicCounter = 0xFFFF;
  }
}

static void PNS_IOTask(NETX_COMM_CHANNEL_HANDLER_RSC_H hRsc)
{
  PNS_RESSOURCES_T *ptRsc = (PNS_RESSOURCES_T*) hRsc;

  long lRet = CIFX_NO_ERROR; /** Return value for common error codes  */
  IO_UpdateCycleCounter(ptRsc);

  /** INPUT DATA *********************************************************************/
  lRet = xChannelIORead(ptRsc->hCifXChannel, 0, 0, sizeof(ptRsc->tInputData), &ptRsc->tInputData, 0);
  if(lRet != CIFX_NO_ERROR)
  {
    /** Something failed?
     * Reason for error could be:
     * 1) netX is not "ready" yet. May happen during startup.
     * 2) netX is not "running" yet. May happen during startup in case the netX is not fully configured yet.
     * 3) netX has not yet established an IO connection. */
  }
  else
  {

  }

  /** OUTPUT DATA ***************************************/
  /** update output data image to be sent in this cycle */

  lRet = xChannelIOWrite(ptRsc->hCifXChannel, 0, 0, sizeof(ptRsc->tOutputData), &ptRsc->tOutputData, 0);
  if(lRet != CIFX_NO_ERROR)
  {
    /** Something failed?
     * Reason for error could be:
     * 1) netX is not "ready" yet. May happen during startup.
     * 2) netX is not "running" yet. May happen during startup in case the netX is not fully configured yet.
     * 3) netX has not yet established an IO connection. */
  }
}

/**************************************************************************************
*   De-Initialize PNS Application
*
*   \param phCommChHdlRsc
**************************************************************************************/
static void PNS_DeInitialize(NETX_COMM_CHANNEL_HANDLER_RSC_H phStackRsc, CIFXHANDLE hCifXChannel)
{
  PRINTF("---------- Free memory and unregister callbacks ----------" NEWLINE);

  uint32_t ulRet = CIFX_NO_ERROR;
  PNS_RESSOURCES_T *ptRsc = (PNS_RESSOURCES_T*)phStackRsc;
  uint32_t            ulState = 0;

  if (NULL == ptRsc) return;

  /* First, turn the bus off */
  SysPkt_AssembleStartStopCommReq(&ptRsc->tPacket, false);
  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &ptRsc->tPacket, TXRX_TIMEOUT);

  if (CIFX_NO_ERROR != (ulRet = xChannelHostState(ptRsc->hCifXChannel, CIFX_HOST_STATE_NOT_READY, &ulState, 5)))
  {
      PRINTF("ERROR: xChannelHostState failed: 0x%08X" NEWLINE, (unsigned int)ulRet);
      return;
  }

  PRINTF("---------- Cleaning finished ----------" NEWLINE);
}


NETX_COMM_CHANNEL_HANDLER_T g_tRealtimeEthernetHandler =
{
  .pfnInitialize    = PNS_Initialize,
  .pfnSetup         = PNS_Setup,
  .pfnCyclicTask    = PNS_IOTask,
  .pfnDeInitialize  = PNS_DeInitialize,
};
