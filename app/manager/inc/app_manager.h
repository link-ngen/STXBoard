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

typedef struct FreeRTOS_THREAD_Ttag FreeRTOS_THREAD_T;
struct FreeRTOS_THREAD_Ttag
{
  TaskHandle_t    hThread;    // Handle to the task object.
  TaskFunction_t  pfnThread;  // Pointer to the thread function.
  void*           pvArg;      // Pointer to the thread function argument.
  FreeRTOS_THREAD_T* netx;
};

void app_init();
void app_run();

#endif /* MANAGER_INC_APP_MANAGER_H_ */
