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

typedef struct AppQueues_tag
{
  QueueHandle_t ledQueue;
} AppQueues_t;

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

typedef struct AppResources_Ttag
{
  AppQueues_t     tAppQueues;
  NetxRessource_t *ptNetxRsc;
} AppResources_t;

void AppManager_Init();
void AppManager_Run();
void AppManager_UpdatePeripherals(NetxRessource_t* ptNetxRsc);

#endif /* MANAGER_INC_APP_MANAGER_H_ */
