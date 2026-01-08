/*
 * netx_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include <stdio.h>
#include "app_config.h"
#include "app_defines.h"
#include "netx_worker.h"

#define IO_CYCLE_TIME     4   /* in milli seconds */

extern NETX_PROTOCOL_DESC_T g_tRealtimeEthernetHandler;

static void State_NetxInit(NETX_APP_RSC_T *ptNetxRsc);
static void State_NetxPreOP(NETX_APP_RSC_T *ptNetxRsc);
static void State_NetxOP(NETX_APP_RSC_T *ptNetxRsc);
static void State_NetxError(NETX_APP_RSC_T *ptNetxRsc);

static const NetxStateDescriptor_t NETX_STATE_INIT_DESC  = { NETX_STATE_INIT,  State_NetxInit  };
static const NetxStateDescriptor_t NETX_STATE_PREOP_DESC = { NETX_STATE_PREOP, State_NetxPreOP };
static const NetxStateDescriptor_t NETX_STATE_OP_DESC    = { NETX_STATE_OP,    State_NetxOP    };
static const NetxStateDescriptor_t NETX_STATE_ERROR_DESC = { NETX_STATE_ERROR, State_NetxError };

static QueueHandle_t s_pxQueue;
static APP_MESSAGE_T s_appMsg;

static void prvFSMTransition(NETX_APP_RSC_T *ptNetxRsc, const NetxStateDescriptor_t *ptNextState)
{
  if(ptNetxRsc->currentState->id == ptNextState->id)
    return;

  ptNetxRsc->previousState = ptNetxRsc->currentState;
  ptNetxRsc->currentState = ptNextState;
}

static NETX_PROTOCOL_RSC_T tNetxProcRsc;
void State_NetxInit(NETX_APP_RSC_T *ptNetxRsc)
{
  PRINTF("---------- NetX Init ----------" NEWLINE);
  int32_t lRet = InitializeToolkit(ptNetxRsc);
  static const NetxStateDescriptor_t *ptNetxStateDesc;

  if (CIFX_NO_ERROR == lRet)
  {
    memset(&tNetxProcRsc, 0, sizeof(NETX_PROTOCOL_RSC_T));
    ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL] = (NETX_PROTOCOL_RSC_T*)&tNetxProcRsc;
    ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->tProtocolDesc = g_tRealtimeEthernetHandler;

#if USED_COMMUNICATION_CHANNELS >= 2
#error "g_tNetworkServicesHandlers must be explicitly implemented!"
    ptNetxRsc->atCommChannels[1] = (NETX_PROTOCOL_RSC_T*)OS_Memcalloc(1, sizeof(NETX_PROTOCOL_RSC_T));
    ptNetxRsc->atCommChannels[1]->tProtocolDesc = g_tNetworkServicesHandlers;
#endif
    lRet = xDriverOpen(&ptNetxRsc->hDriver);
    if (CIFX_NO_ERROR != lRet)
    {
      s_appMsg.eCommand = APP_CMD_NETX_INIT_ERR;
      ptNetxStateDesc = &NETX_STATE_ERROR_DESC;
    }
    else /* CifXToolkit driver open succeed */
    {
      s_appMsg.eCommand = APP_CMD_NETX_INIT_OK;
      ptNetxStateDesc = &NETX_STATE_PREOP_DESC;
    }
    AppManager_SendCommand(&s_appMsg);
  }
  else
  {
    ptNetxStateDesc = &NETX_STATE_ERROR_DESC;
  }
  prvFSMTransition(ptNetxRsc, ptNetxStateDesc);
  ptNetxRsc->previousState = &NETX_STATE_INIT_DESC;
}

void State_NetxPreOP(NETX_APP_RSC_T *ptNetxRsc)
{
  PRINTF("---------- NetX PreOP ----------" NEWLINE);

  if(!(ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL])->fDeviceIsRunning)
  {
    if((CIFX_NO_ERROR != NetX_InitializeChannels(ptNetxRsc, "cifX0")) ||
      (CIFX_NO_ERROR != NetX_ConfigureChannels(ptNetxRsc)))
    {
      ++ptNetxRsc->bInitErrCounter;
      s_appMsg.eCommand = APP_CMD_NETX_INIT_OK;
      AppManager_SendCommand(&s_appMsg);
      prvFSMTransition(ptNetxRsc, &NETX_STATE_ERROR_DESC);
      return;
    }
  }

  static const NetxStateDescriptor_t *ptNetxStateDesc;
  /* Start mailbox communication */
  (void) vTaskResume(ptNetxRsc->xMailboxTaskHandle);

  /* config done */
  uint32_t commState = Netx_ReadNetworkState(ptNetxRsc);
  /*XXX: To access the operating mode, some hilscher firmware requires the xChannelIORead or Write function.  */
  if(commState & HIL_COMM_STATE_OPERATE)
  {
    s_appMsg.eCommand = APP_CMD_NETX_PLC_CONNECTED;
    ptNetxStateDesc = &NETX_STATE_OP_DESC;
    xTimerChangePeriod(ptNetxRsc->hCyclicTimer, pdMS_TO_TICKS(IO_CYCLE_TIME), 1);
  }
  else /* if stack is not in OP -> no communication with a PLC or controller */
  {
    s_appMsg.eCommand = APP_CMD_NETX_PLC_DISCONNECTED;
    ptNetxStateDesc = &NETX_STATE_PREOP_DESC;
  }
  AppManager_SendCommand(&s_appMsg);
  prvFSMTransition(ptNetxRsc, ptNetxStateDesc);
  ptNetxRsc->previousState = &NETX_STATE_PREOP_DESC;
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
      s_appMsg.eCommand = APP_CMD_NETX_UPDATE_IODATA;
      s_appMsg.pvData = ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->abActorData;
      ptNetxStateDesc = &NETX_STATE_OP_DESC;
      break;

    case CIFX_DEV_NO_COM_FLAG:
      s_appMsg.eCommand = APP_CMD_NETX_PLC_DISCONNECTED;
      ptNetxStateDesc = &NETX_STATE_PREOP_DESC;
      xTimerChangePeriod(ptNetxRsc->hCyclicTimer, pdMS_TO_TICKS(1), 1);
      break;

    case CIFX_DEV_EXCHANGE_FAILED:
    default:
      s_appMsg.eCommand = APP_CMD_NETX_DPM_ERR;
      ptNetxStateDesc = &NETX_STATE_ERROR_DESC;
      xTimerChangePeriod(ptNetxRsc->hCyclicTimer, pdMS_TO_TICKS(1), 1);
      break;
    }
  }

  AppManager_SendCommand(&s_appMsg);
  prvFSMTransition(ptNetxRsc, ptNetxStateDesc);
  ptNetxRsc->previousState = &NETX_STATE_OP_DESC;
}

void State_NetxError(NETX_APP_RSC_T *ptNetxRsc)
{
  PRINTF("---------- NetX Error ----------" NEWLINE);
  /* XXX: This is a quick solution. Think about reinitialize cifxtoolkit, if netx handshake freeze. */
  if(ptNetxRsc->previousState->id == NETX_STATE_PREOP &&
    ptNetxRsc->bInitErrCounter < 64)
  {
    ptNetxRsc->fNetXDrvRunning = false;
    s_appMsg.eCommand = APP_CMD_NETX_INIT_ERR;
    prvFSMTransition(ptNetxRsc, &NETX_STATE_PREOP_DESC);
  }
  else
  {
    s_appMsg.eCommand = APP_CMD_NETX_GENERAL_ERR;
    prvFSMTransition(ptNetxRsc, &NETX_STATE_ERROR_DESC);
    vTaskSuspend(ptNetxRsc->xMailboxTaskHandle);
  }
  AppManager_SendCommand(&s_appMsg);
  ptNetxRsc->previousState = &NETX_STATE_ERROR_DESC;
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

bool NetX_SendSensorUpdate(const void *pvData, TickType_t xTicksToWait)
{
  UNUSED(xTicksToWait);
  uint8_t *pbData = (uint8_t*) pvData;

  if(s_pxQueue == NULL || pvData == NULL)
  {
    return false;
  }
  return (xQueueOverwrite(s_pxQueue, pbData) == pdPASS);
}

static NETX_APP_RSC_T tNetxFSM;
static void CyclicTimerCallback(TimerHandle_t xTimer)
{
  (void) xTaskNotifyGive(tNetxFSM.xNetxWorkerTaskHandle);
}

void NetxWorker(void *pvParameters)
{
  tNetxFSM.currentState   = &NETX_STATE_INIT_DESC;
  tNetxFSM.previousState  = &NETX_STATE_INIT_DESC;
  s_pxQueue = (QueueHandle_t)pvParameters;

  BaseType_t xReturned = pdPASS;

  tNetxFSM.xNetxWorkerTaskHandle = xTaskGetCurrentTaskHandle();
  tNetxFSM.hCyclicTimer = xTimerCreate("CyclicTimer",
                          pdMS_TO_TICKS(1),
                          pdTRUE,
                          NULL,
                          CyclicTimerCallback);

  /* Create mailbox task */
  xReturned = xTaskCreate((pdTASK_CODE)NetX_MailboxTask,
                          "MbxTask",
                          configMINIMAL_STACK_SIZE * 3,
                          &tNetxFSM,
                          (tskIDLE_PRIORITY) + 1,
                          &tNetxFSM.xMailboxTaskHandle);

  configASSERT(pdPASS == xReturned);
  vTaskSuspend(tNetxFSM.xMailboxTaskHandle);
  xTimerStart(tNetxFSM.hCyclicTimer, 0);

  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

#if 0
    /* Get new data from the sensor */
    xQueueReceive(s_pxQueue, tNetxFSM.atCommChannels[REALTIME_ETH_CHANNEL]->abSensorData, 0);
#endif

    /* Main state machine logic, runs the current state function */
    tNetxFSM.currentState->pfnc(&tNetxFSM);
  }
}
