/*
 * app_manager.h
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#ifndef MANAGER_INC_APP_MANAGER_H_
#define MANAGER_INC_APP_MANAGER_H_

#include "app_defines.h"
#include "task_messaging.h"
#include "netx_worker.h"

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
  QUEUE_CONFIG_T    ptTaskQueueConfig[QUEUE_ID_COUNT];
  APP_COMMAND_E     eCurrentCmd;
  LED_COMMAND_E     tLedCmd;
  LCD_COMMAND_T     tLcdCommand;
  bool              fInitialized;
} APP_MANANGER_RSC_T;

void AppManager_Init();
void AppManager_Run();

#endif /* MANAGER_INC_APP_MANAGER_H_ */
