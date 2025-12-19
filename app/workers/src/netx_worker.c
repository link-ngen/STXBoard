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

#define IO_CYCLE_TIME     4   /* in milli seconds */

extern NETX_PROTOCOL_DESC_T g_tRealtimeEthernetHandler;
extern NETX_PROTOCOL_DESC_T g_tNetworkServicesHandlers;

static void State_NetxInit(NETX_APP_RSC_T *ptNetxRsc);
static void State_NetxPreOP(NETX_APP_RSC_T *ptNetxRsc);
static void State_NetxOP(NETX_APP_RSC_T *ptNetxRsc);
static void State_NetxError(NETX_APP_RSC_T *ptNetxRsc);

static const NetxStateDescriptor_t NETX_STATE_INIT_DESC  = { NETX_STATE_INIT,  State_NetxInit  };
static const NetxStateDescriptor_t NETX_STATE_PREOP_DESC = { NETX_STATE_PREOP, State_NetxPreOP };
static const NetxStateDescriptor_t NETX_STATE_OP_DESC    = { NETX_STATE_OP,    State_NetxOP    };
static const NetxStateDescriptor_t NETX_STATE_ERROR_DESC = { NETX_STATE_ERROR, State_NetxError };

static void NetX_OnEnterState(NETX_APP_RSC_T *ptNetxRsc, NetxStateId_e tNextStateId)
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

static void NetX_FSMTransition(NETX_APP_RSC_T *ptNetxRsc, const NetxStateDescriptor_t *ptNextState)
{
  if(ptNetxRsc->currentState->id == ptNextState->id)
    return;

  ptNetxRsc->previousState = ptNetxRsc->currentState;
  ptNetxRsc->currentState = ptNextState;

  NetX_OnEnterState(ptNetxRsc, ptNextState->id);
  AppManager_UpdatePeripherals(ptNetxRsc);
}

void State_NetxInit(NETX_APP_RSC_T *ptNetxRsc)
{
  PRINTF("---------- NetX Init ----------" NEWLINE);
  int32_t lRet = InitializeToolkit(ptNetxRsc);
  static const NetxStateDescriptor_t *ptNetxStateDesc;

  if (CIFX_NO_ERROR == lRet)
  {
    ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL] = (NETX_PROTOCOL_RSC_T*)OS_Memcalloc(1, sizeof(NETX_PROTOCOL_RSC_T));
    ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->tProtocolDesc = g_tRealtimeEthernetHandler;

#if USED_COMMUNICATION_CHANNELS >= 2
    ptNetxRsc->atCommChannels[1] = (NETX_PROTOCOL_RSC_T*)OS_Memcalloc(1, sizeof(NETX_PROTOCOL_RSC_T));
    ptNetxRsc->atCommChannels[1]->tProtocolDesc = g_tNetworkServicesHandlers;
#endif
    lRet = xDriverOpen(&ptNetxRsc->hDriver);
    if (CIFX_NO_ERROR != lRet)
    {
      snprintf(ptNetxRsc->tLcdCommand.pcMessage, sizeof(ptNetxRsc->tLcdCommand.pcMessage), "DrvOpen err \n");
      ptNetxStateDesc = &NETX_STATE_ERROR_DESC;
    }
    else /* CifXToolkit driver open succeed */
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

void State_NetxPreOP(NETX_APP_RSC_T *ptNetxRsc)
{
  PRINTF("---------- NetX PreOP ----------" NEWLINE);

  if(!(ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL])->fDeviceIsRunning)
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

void State_NetxOP(NETX_APP_RSC_T *ptNetxRsc)
{
  PRINTF("---------- NetX OP ----------" NEWLINE);
  static int32_t lRet = CIFX_NO_ERROR;
  static const NetxStateDescriptor_t *ptNetxStateDesc;
  const NETX_PROTOCOL_DESC_T *ptComChannelHandler = &ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->tProtocolDesc;

  if (NULL != ptComChannelHandler->pfnCyclicTask)
  {
    /* Get data from field bus */
    lRet = ptComChannelHandler->pfnCyclicTask(ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]);

    switch (lRet)
    {
    case CIFX_NO_ERROR:
      /* Process input data and prepare for the next write to the field bus */
      // TODO: process io data. Send input data to other task and get the output data from other task
      NetX_ProcessIOData(ptNetxRsc);

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

void State_NetxError(NETX_APP_RSC_T *ptNetxRsc)
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
    vTaskSuspend(ptNetxRsc->xMailboxTaskHandle);
  }
  ptNetxRsc->previousState = &NETX_STATE_ERROR_DESC;
  vTaskDelay(pdMS_TO_TICKS(1));
}

static void NetX_MailboxTask(void* pvParameters)
{
  NETX_APP_RSC_T *ptNetxRsc = (NETX_APP_RSC_T*)pvParameters;
  while (1)
  {
    NetX_CallCommMailboxRoutine(ptNetxRsc);
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void NetxWorker(void *pvParameters)
{
  NETX_APP_RSC_T *ptNetxFSM = OS_Memalloc(sizeof(NETX_APP_RSC_T));
  OS_Memset(ptNetxFSM, 0, sizeof(NETX_APP_RSC_T));

  ptNetxFSM->currentState   = &NETX_STATE_INIT_DESC;
  ptNetxFSM->previousState  = &NETX_STATE_INIT_DESC;
  pvParameters                = (void*)ptNetxFSM;

  BaseType_t xReturned = pdPASS;

  /* Create mailbox task */
  xReturned = xTaskCreate((pdTASK_CODE)NetX_MailboxTask,
                          "MbxTask",
                          configMINIMAL_STACK_SIZE * 3,
                          ptNetxFSM,
                          (tskIDLE_PRIORITY) + 1,
                          &ptNetxFSM->xMailboxTaskHandle);

  configASSERT(pdPASS == xReturned);
  vTaskSuspend(ptNetxFSM->xMailboxTaskHandle);

  NetX_OnEnterState(ptNetxFSM, NETX_STATE_INIT);

  while (1)
  {
    /* Main state machine logic, runs the current state function */
    ptNetxFSM->currentState->pfnc(ptNetxFSM);
  }
}
