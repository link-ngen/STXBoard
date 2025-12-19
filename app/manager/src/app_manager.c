/*
 * app_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include "app_manager.h"
#include "led_worker.h"
#include "lcd_worker.h"

static AppResources_t *s_ptAppRsc;

void AppManager_Init()
{
  s_ptAppRsc = OS_Memalloc(sizeof(AppResources_t));
  OS_Memset(s_ptAppRsc, 0, sizeof(AppResources_t));

  s_ptAppRsc->tAppQueues.ledQueue = xQueueCreate(LED_QUEUE_LEN, sizeof(eLedCommand));

  FreeRTOS_THREAD_T taskConfigs[] = {
    { (pdTASK_CODE)NetxWorker, "netx90 Task", configMINIMAL_STACK_SIZE * 24, (void*)s_ptAppRsc->ptNetxRsc, (tskIDLE_PRIORITY) + 2, NULL },
    { (pdTASK_CODE)LCD_Worker, "LCD Task", configMINIMAL_STACK_SIZE * 4, NULL, (tskIDLE_PRIORITY) + 0, NULL },
    { (pdTASK_CODE)LED_Worker, "Conf Led Task", configMINIMAL_STACK_SIZE, (void*)s_ptAppRsc->tAppQueues.ledQueue, (tskIDLE_PRIORITY) + 3, NULL },
  };

  BaseType_t xReturned = pdPASS;

  for (uint32_t i = 0; i < sizeof(taskConfigs) / sizeof(taskConfigs[0]); ++i)
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

void AppManager_Run()
{
  vTaskStartScheduler();
  /* catch errors */
}

void AppManager_UpdatePeripherals(NETX_APP_RSC_T* ptNetxRsc)
{
  (void)LED_SendCommand(s_ptAppRsc->tAppQueues.ledQueue, &ptNetxRsc->tLedCmd);
  (void)LCD_SendCommand(&ptNetxRsc->tLcdCommand);
}
