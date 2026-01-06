/*
 * system_queue.c
 *
 *  Created on: 05.01.2026
 *      Author: nnguyen
 */
#include <string.h>
#include "app_defines.h"
#include "task_messaging.h"

#include "led_worker.h"
#include "lcd_worker.h"
#include "neopixel_worker.h"

static SemaphoreHandle_t s_xTaskMsgMutex = NULL;
static QUEUE_CONFIG_T *s_ptQueueTable = NULL;
/**
 * @brief Interne Funktion: Validiert Queue-ID
 */
static bool prvIsValidQueueId(MSG_QUEUE_ID_E eId)
{
  return (eId < QUEUE_ID_COUNT);
}

/* Private functions */
/**
 * @brief Interne Funktion: Holt Queue-Handle mit Prüfung
 */
static QueueHandle_t prvGetQueueHandle(MSG_QUEUE_ID_E eId)
{
  if(!prvIsValidQueueId(eId))
  {
    return NULL;
  }

  QueueHandle_t hQueue = s_ptQueueTable[eId].handle;
  if(NULL != hQueue)
  {
    UBaseType_t uxLength = uxQueueMessagesWaiting(hQueue);
    (void) uxLength;
  }

  return hQueue;
}

BaseType_t TaskMsg_Init(QUEUE_CONFIG_T *ptQueueConfig)
{
  s_xTaskMsgMutex = xSemaphoreCreateMutex();
  if (NULL == s_xTaskMsgMutex)
  {
    return pdFAIL;
  }

  /* create queues */
  for (MSG_QUEUE_ID_E queueId = QUEUE_ID_LED_WORKER; queueId < QUEUE_ID_COUNT; ++queueId)
  {
    ptQueueConfig[queueId].handle = xQueueCreate(ptQueueConfig[queueId].length, ptQueueConfig[queueId].itemSize);

    if (NULL == ptQueueConfig[queueId].handle)
    {
      /* if something wrong, delete all created queue */
      for (MSG_QUEUE_ID_E createdQueue = QUEUE_ID_LED_WORKER; createdQueue < queueId; ++createdQueue)
      {
        if (NULL != ptQueueConfig[createdQueue].handle)
        {
          vQueueDelete(ptQueueConfig[createdQueue].handle);
          ptQueueConfig[createdQueue].handle = NULL;
        }
      }

      vSemaphoreDelete(s_xTaskMsgMutex);
      s_xTaskMsgMutex = NULL;
      return pdFAIL;
    }
  }
  s_ptQueueTable = ptQueueConfig;
  return pdTRUE;
}

QueueHandle_t TaskMsg_GetQueue(MSG_QUEUE_ID_E eReceiverId)
{
  if (xSemaphoreTake(s_xTaskMsgMutex, pdMS_TO_TICKS(10)) == pdTRUE)
  {
    QueueHandle_t hQueue = prvGetQueueHandle(eReceiverId);
    xSemaphoreGive(s_xTaskMsgMutex);
    return hQueue;
  }
  return NULL;
}

BaseType_t TaskMsg_SendTo(MSG_QUEUE_ID_E eReceiverId, const void *pvData, TickType_t xTicksToWait)
{
  BaseType_t xResult = pdFAIL;
  if(!prvIsValidQueueId(eReceiverId) || NULL == pvData)
  {
    return pdFAIL;
  }

  if (xSemaphoreTake(s_xTaskMsgMutex, pdMS_TO_TICKS(10)) != pdTRUE)
  {
    return pdFAIL;
  }

  QueueHandle_t hQueue = prvGetQueueHandle(eReceiverId);
  if (NULL != hQueue)
  {
    xResult = s_ptQueueTable[eReceiverId].pfnHandler(pvData);
  }

  xSemaphoreGive(s_xTaskMsgMutex);
  return xResult;
}

void TaskMsg_Deinit(void)
{

}
