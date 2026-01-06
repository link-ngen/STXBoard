/*
 * system_queue.h
 *
 *  Created on: 05.01.2026
 *      Author: nnguyen
 */

#ifndef MANAGER_INC_TASK_MESSAGING_H_
#define MANAGER_INC_TASK_MESSAGING_H_

#include <stdint.h>
#include <stdbool.h>
#include "app_defines.h"

BaseType_t TaskMsg_Init(QUEUE_CONFIG_T *ptQueueConfig);
QueueHandle_t TaskMsg_GetQueue(MSG_QUEUE_ID_E eReceiverId);
BaseType_t TaskMsg_SendTo(MSG_QUEUE_ID_E eReceiverId, const void *pvData, TickType_t xTicksToWait);
void TaskMsg_Deinit(void);

#endif /* MANAGER_INC_TASK_MESSAGING_H_ */
