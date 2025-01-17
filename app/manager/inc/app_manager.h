/*
 * app_manager.h
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#ifndef MANAGER_INC_APP_MANAGER_H_
#define MANAGER_INC_APP_MANAGER_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef struct AppQueues_tag
{
  QueueHandle_t lcdQueue;
  QueueHandle_t ledQueue;
} AppQueues_t;

typedef struct FreeRTOS_THREAD_Ttag FreeRTOS_THREAD_T;
struct FreeRTOS_THREAD_Ttag
{
  TaskFunction_t      pfnThread;  // Pointer to the thread function.
  const char          *pcName;
  uint16_t            usStackDepth;
  void                *pvArg;      // Pointer to the thread function argument.
  UBaseType_t         uxPriority;
  TaskHandle_t        hThread;    // Handle to the task object.
};

void app_init();
void app_run();

#endif /* MANAGER_INC_APP_MANAGER_H_ */
