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
#include "netx_functions.h"

#define IO_CYCLE_TIME     4   /* in milli seconds */

extern NETX_COMM_CHANNEL_HANDLER_T g_tRealtimeEthernetHandler;
extern NETX_COMM_CHANNEL_HANDLER_T g_tNetworkServicesHandlers;

static NetxRessource_t *s_ptNetxFSM;

static void State_NetxInit(NetxRessource_t *ptNetxRsc);
static void State_NetxPreOP(NetxRessource_t *ptNetxRsc);
static void State_NetxOP(NetxRessource_t *ptNetxRsc);
static void State_NetxError(NetxRessource_t *ptNetxRsc);

static const NetxStateDescriptor_t NETX_STATE_INIT_DESC  = { NETX_STATE_INIT,  State_NetxInit  };
static const NetxStateDescriptor_t NETX_STATE_PREOP_DESC = { NETX_STATE_PREOP, State_NetxPreOP };
static const NetxStateDescriptor_t NETX_STATE_OP_DESC    = { NETX_STATE_OP,    State_NetxOP    };
static const NetxStateDescriptor_t NETX_STATE_ERROR_DESC = { NETX_STATE_ERROR, State_NetxError };

static void NetX_OnEnterState(NetxRessource_t *ptNetxRsc, NetxStateId_e tNextStateId)
{
  switch (tNextStateId)
  {
  case NETX_STATE_INIT:
    ptNetxRsc->tLedCmd = LED_CMD_CONFIGURING;
    ptNetxRsc->tLcdCommand.eScreen = LCD_CONFIG_SCREEN;
    break;

  case NETX_STATE_PREOP:
    ptNetxRsc->tLedCmd = LED_CMD_CONFIGURED;
    ptNetxRsc->tLcdCommand.eScreen = LCD_VERTEX_SCREEN;
    break;

  case NETX_STATE_OP:
    ptNetxRsc->tLedCmd = LED_CMD_RUN_ON;
    ptNetxRsc->tLcdCommand.eScreen = LCD_IOXCHANGE_SCREEN;
    break;

  case NETX_STATE_ERROR:
  default:
    ptNetxRsc->tLedCmd = LED_CMD_ERROR_ON;
    ptNetxRsc->tLcdCommand.eScreen = LCD_ERROR_SCREEN;
    break;
  }
}

static void NetX_FSMTransition(NetxRessource_t *ptNetxRsc, const NetxStateDescriptor_t *ptNextState)
{
  if(ptNetxRsc->currentState->id == ptNextState->id)
    return;

  ptNetxRsc->previousState = ptNetxRsc->currentState;
  ptNetxRsc->currentState = ptNextState;

  NetX_OnEnterState(ptNetxRsc, ptNextState->id);
  AppManager_UpdatePeripherals(ptNetxRsc);
}

void State_NetxInit(NetxRessource_t *ptNetxRsc)
{
  PRINTF("---------- NetX Init ----------" NEWLINE);
  int32_t lRet = InitializeToolkit(ptNetxRsc);
  static const NetxStateDescriptor_t *ptNetxStateDesc;

  if (CIFX_NO_ERROR == lRet)
  {
    ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL].tCommChannelHandler = g_tRealtimeEthernetHandler;
#if USED_COMMUNICATION_CHANNELS >= 2
    ptNetxRsc->atCommChannels[1].tCommChannelHandler = g_tNetworkServicesHandlers;
#endif
    lRet = xDriverOpen(&ptNetxRsc->hDriver);
    if (CIFX_NO_ERROR != lRet)
    {
      snprintf(ptNetxRsc->tLcdCommand.pcMessage, sizeof(ptNetxRsc->tLcdCommand.pcMessage), "DrvOpen err \n");
      ptNetxStateDesc = &NETX_STATE_ERROR_DESC;
    }
    else
    {
      snprintf(ptNetxRsc->tLcdCommand.pcMessage, sizeof(ptNetxRsc->tLcdCommand.pcMessage), "DrvOpen ok\n");
      (void) vTaskResume(ptNetxRsc->xMailboxTaskHandle);
      ptNetxStateDesc = &NETX_STATE_PREOP_DESC;
    }
  }
  else
  {
    ptNetxStateDesc = &NETX_STATE_ERROR_DESC;
  }
  NetX_FSMTransition(ptNetxRsc, ptNetxStateDesc);
  ptNetxRsc->previousState = &NETX_STATE_INIT_DESC;
  vTaskDelay(pdMS_TO_TICKS(1));
}

void State_NetxPreOP(NetxRessource_t *ptNetxRsc)
{
  PRINTF("---------- NetX PreOP ----------" NEWLINE);

  if(!((PNS_RESSOURCES_T*) ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL].hCommChannelRsc)->fDeviceIsRunning)
  {
    if((CIFX_NO_ERROR != NetX_InitializeChannels(ptNetxRsc, "cifX0")) ||
      (CIFX_NO_ERROR != NetX_ConfigureChannels(ptNetxRsc)))
    {
      snprintf(ptNetxRsc->tLcdCommand.pcMessage, sizeof(ptNetxRsc->tLcdCommand.pcMessage), "NetX config error\n");
      ++ptNetxRsc->bInitErrCounter;
      NetX_FSMTransition(ptNetxRsc, &NETX_STATE_ERROR_DESC);
      return;
    }
  }

  static const NetxStateDescriptor_t *ptNetxStateDesc;

  /* config done */
  uint32_t commState = Netx_ReadNetworkState(ptNetxRsc);
  if(commState & HIL_COMM_STATE_OPERATE)
  {
    ptNetxStateDesc = &NETX_STATE_OP_DESC;
  }
  else /* if stack is not in OP -> no communication with a PLC or controller */
  {
    ptNetxStateDesc = &NETX_STATE_PREOP_DESC;
  }

  NetX_FSMTransition(ptNetxRsc, ptNetxStateDesc);
  ptNetxRsc->previousState = &NETX_STATE_PREOP_DESC;
  vTaskDelay(pdMS_TO_TICKS(1));
}

void State_NetxOP(NetxRessource_t *ptNetxRsc)
{
  PRINTF("---------- NetX OP ----------" NEWLINE);
  static int32_t lRet = CIFX_NO_ERROR;
  static const NetxStateDescriptor_t *ptNetxStateDesc;
  const NETX_COMM_CHANNEL_HANDLER_T *ptComChannelHandler = &ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL].tCommChannelHandler;

  if (NULL != ptComChannelHandler->pfnCyclicTask)
  {
    /* Get data from field bus */
    lRet = ptComChannelHandler->pfnCyclicTask(ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL].hCommChannelRsc);

    switch (lRet)
    {
    case CIFX_NO_ERROR:
      /* Process input data and prepare for the next write to the field bus */
      // TODO: process io data. Send input data to other task and get the output data from other task
      ProcessIOData(ptNetxRsc);

      ptNetxStateDesc = &NETX_STATE_OP_DESC;
      break;

    case CIFX_DEV_NO_COM_FLAG:
      ptNetxStateDesc = &NETX_STATE_PREOP_DESC;
      break;

    case CIFX_DEV_EXCHANGE_FAILED:
    default:
      snprintf(ptNetxRsc->tLcdCommand.pcMessage, sizeof(ptNetxRsc->tLcdCommand.pcMessage), "ChnCom failed \n");
      ptNetxStateDesc = &NETX_STATE_ERROR_DESC;
      break;
    }
  }

  NetX_FSMTransition(ptNetxRsc, ptNetxStateDesc);
  ptNetxRsc->previousState = &NETX_STATE_OP_DESC;

  vTaskDelay(pdMS_TO_TICKS(IO_CYCLE_TIME)); /* 4ms io cyclic task */
}

void State_NetxError(NetxRessource_t *ptNetxRsc)
{
  PRINTF("---------- NetX Error ----------" NEWLINE);

  if(ptNetxRsc->previousState->id == NETX_STATE_PREOP &&
    ptNetxRsc->bInitErrCounter < 64)
  {
    ptNetxRsc->fNetXDrvRunning = false;
    snprintf(ptNetxRsc->tLcdCommand.pcMessage, sizeof(ptNetxRsc->tLcdCommand.pcMessage), "%s", "Err\n");
    NetX_FSMTransition(ptNetxRsc, &NETX_STATE_PREOP_DESC);
  }
  else
  {
    NetX_FSMTransition(ptNetxRsc, &NETX_STATE_ERROR_DESC);

    /* Manually update peripherals */
    AppManager_UpdatePeripherals(ptNetxRsc);
    vTaskSuspend(s_ptNetxFSM->xMailboxTaskHandle);
  }
  ptNetxRsc->previousState = &NETX_STATE_ERROR_DESC;
  vTaskDelay(pdMS_TO_TICKS(1));
}

static void NetX_MailboxTask(void* pvParameters)
{
  NetxRessource_t *ptNetxRsc = (NetxRessource_t*)pvParameters;
  while (1)
  {
    NetX_CallCommMailboxRoutine(ptNetxRsc);
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void NetxWorker(void *pvParameters)
{
  s_ptNetxFSM = OS_Memalloc(sizeof(NetxRessource_t));
  OS_Memset(s_ptNetxFSM, 0, sizeof(NetxRessource_t));

  s_ptNetxFSM->currentState   = &NETX_STATE_INIT_DESC;
  s_ptNetxFSM->previousState  = &NETX_STATE_INIT_DESC;
  pvParameters                = (void*)s_ptNetxFSM;

  BaseType_t xReturned = pdPASS;

  /* Create mailbox task */
  xReturned = xTaskCreate((pdTASK_CODE)NetX_MailboxTask,
                          "MbxTask",
                          configMINIMAL_STACK_SIZE * 3,
                          s_ptNetxFSM,
                          (tskIDLE_PRIORITY) + 1,
                          &s_ptNetxFSM->xMailboxTaskHandle);

  configASSERT(pdPASS == xReturned);
  vTaskSuspend(s_ptNetxFSM->xMailboxTaskHandle);

  NetX_OnEnterState(s_ptNetxFSM, NETX_STATE_INIT);

  while (1)
  {
    /* Main state machine logic, runs the current state function */
    s_ptNetxFSM->currentState->pfnc(s_ptNetxFSM);
  }
}
