/*
 * system_defines.h
 *
 *  Created on: 05.01.2026
 *      Author: nnguyen
 */

#ifndef MANAGER_INC_APP_DEFINES_H_
#define MANAGER_INC_APP_DEFINES_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

#define MSG_LED_QUEUE_LEN     1
#define MSG_LCD_QUEUE_LEN     1
#define MSG_NEOPXL_QUEUE_LEN  1

typedef bool (*WorkerSendCmdCallback)(const void *pvUserData);
typedef struct
{
  QueueHandle_t handle;
  uint32_t length;
  uint32_t itemSize;
  WorkerSendCmdCallback pfnHandler;
} QUEUE_CONFIG_T;

/**
 * @brief All available queues in the system
 */
typedef enum
{
  /* Management Queues */
//  QUEUE_ID_APP_MANAGER=0,          ///< Main application management queue
  /* Worker Queues */
  QUEUE_ID_LED_WORKER,           ///< LED control
  QUEUE_ID_LCD_WORKER,           ///< LCD display
  QUEUE_ID_NEOPIXEL_WORKER,      ///< Neopixel/LED strip control
  QUEUE_ID_COUNT,                ///< Total number of queues (MUST be last element!)
} MSG_QUEUE_ID_E;


/* Debug function enable */
// #define DBG_ENABLE_LOGGING

#endif /* MANAGER_INC_APP_DEFINES_H_ */
