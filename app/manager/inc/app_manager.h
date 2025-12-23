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

#include "netx_worker.h"

typedef struct APP_QUEUE_Ttag
{
  QueueHandle_t ledQueue;
  QueueHandle_t neopixelQueue;
} APP_QUEUE_T;

typedef struct FreeRTOS_THREAD_Ttag FreeRTOS_THREAD_T;
struct FreeRTOS_THREAD_Ttag
{
  TaskFunction_t      pfnThread;    // Pointer to the thread function.
  const char          *pcName;
  uint16_t            usStackDepth;
  void                *pvArg;       // Pointer to the thread function argument.
  UBaseType_t         uxPriority;
  TaskHandle_t        hThread;      // Handle to the task object.
};

typedef struct APP_MANANGER_RSC_Ttag
{
  APP_QUEUE_T     tAppQueues;
  NETX_APP_RSC_T *ptNetxRsc;
} APP_MANANGER_RSC_T;

void AppManager_Init();
void AppManager_Run();
void AppManager_UpdatePeripherals(NETX_APP_RSC_T* ptNetxRsc);
void AppManager_UpdateNeopixel(NETX_APP_RSC_T* ptNetxRsc);
void AppManager_Call_Flashing_Mode(NETX_APP_RSC_T* ptNetxRsc);
#endif /* MANAGER_INC_APP_MANAGER_H_ */
