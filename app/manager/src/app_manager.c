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
//  FreeRTOS_THREAD_T lcdTaskHandle;
//
//  lcdTaskHandle.pfnThread = (pdTASK_CODE) LCD_OctahedronWorker;
//  lcdTaskHandle.pvArg = NULL;
//
//  BaseType_t xReturned = xTaskCreate(lcdTaskHandle.pfnThread, /* Function that implements the task. */
//                                    "LCD Task", /* Text name for the task. */
//                                    configMINIMAL_STACK_SIZE * 2, /* Stack size in words, not bytes. */
//                                    (void*) &lcdTaskHandle, /* Parameter passed into the task. */
//                                    (tskIDLE_PRIORITY) + 1, /* Priority at which the task is created. */
//                                    &lcdTaskHandle.hThread /* Used to pass out the created task's handle. */
//                                    );


  FreeRTOS_THREAD_T netx90TaskHandle;

  netx90TaskHandle.pfnThread = (pdTASK_CODE) NetxDemoWorker;
  netx90TaskHandle.pvArg = NULL;

  BaseType_t xReturned = xTaskCreate(netx90TaskHandle.pfnThread, /* Function that implements the task. */
                          "netx90 Task", /* Text name for the task. */
                          configMINIMAL_STACK_SIZE * 16, /* Stack size in words, not bytes. */
                          (void*) &netx90TaskHandle, /* Parameter passed into the task. */
                          (tskIDLE_PRIORITY) + 2, /* Priority at which the task is created. */
                          &netx90TaskHandle.hThread /* Used to pass out the created task's handle. */
                          );

  configASSERT(pdPASS == xReturned);
}

void app_run()
{
  vTaskStartScheduler();
  /* catch errors */
}
