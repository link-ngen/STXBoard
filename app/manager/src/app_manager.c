/*
 * app_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include "app_manager.h"
#include "netx_worker.h"

AppQueues_t tAppQueues;

void app_init()
{
  tAppQueues.lcdQueue = xQueueCreate(LCD_STATUS_QUEUE_LEN, sizeof(LcdPacket_t));
  tAppQueues.ledQueue = xQueueCreate(LED_QUEUE_LEN, sizeof(LedTaskCommand_t));

  FreeRTOS_THREAD_T taskConfigs[] = {
    { (pdTASK_CODE)NetxWorker, "netx90 Task", configMINIMAL_STACK_SIZE * 24, (void*)&tAppQueues, (tskIDLE_PRIORITY) + 2, NULL },
    { (pdTASK_CODE)LCD_Worker, "LCD Task", configMINIMAL_STACK_SIZE * 4, (void*)tAppQueues.lcdQueue, (tskIDLE_PRIORITY) + 0, NULL },
    { (pdTASK_CODE)LED_Worker, "Conf Led Task", configMINIMAL_STACK_SIZE, (void*)tAppQueues.ledQueue, (tskIDLE_PRIORITY) + 1, NULL },
  };

  BaseType_t xReturned = pdPASS;

  for (int i = 0; i < sizeof(taskConfigs) / sizeof(taskConfigs[0]); ++i)
  {
      xReturned = xTaskCreate( taskConfigs[i].pfnThread,
                               taskConfigs[i].pcName,
                               taskConfigs[i].usStackDepth,
                               taskConfigs[i].pvArg,
                               taskConfigs[i].uxPriority,
                               &taskConfigs[i].hThread);
      configASSERT(pdPASS == xReturned);
  }
}

void app_run()
{
  vTaskStartScheduler();

  /* catch errors */
}
