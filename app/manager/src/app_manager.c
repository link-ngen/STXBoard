/*
 * app_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include "app_manager.h"
#include "netx_worker.h"

void app_init()
{
  /* TODO: issue !!*/
  AppQueues_t appQueues;
  appQueues.lcdQueue = xQueueCreate(LCD_QUEUE_LEN, sizeof(LcdTaskScreen_t));
  appQueues.ledQueue = xQueueCreate(LED_QUEUE_LEN, sizeof(LedTaskCommand_t));

  FreeRTOS_THREAD_T taskConfigs[] = {
    { (pdTASK_CODE)NetxDemoWorker, "netx90 Task", configMINIMAL_STACK_SIZE * 24, (void*)&appQueues, (tskIDLE_PRIORITY) + 2, NULL },
    { (pdTASK_CODE)LCD_Worker, "LCD Task", configMINIMAL_STACK_SIZE * 2, (void*)&appQueues.lcdQueue, (tskIDLE_PRIORITY) + 1, NULL },
    { (pdTASK_CODE)LED_Worker, "Conf Led Task", configMINIMAL_STACK_SIZE, (void*)&appQueues.ledQueue, (tskIDLE_PRIORITY) + 0, NULL },
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
