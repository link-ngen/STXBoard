/*
 * led_worker.h
 *
 *  Created on: Dec 17, 2024
 *      Author: progr
 */

#ifndef WORKERS_INC_LED_WORKER_H_
#define WORKERS_INC_LED_WORKER_H_

#include "led_func.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef enum {
  LED_STATUS_CONFIG_BLINK,
  LED_STATUS_CONFIG_ERROR,
  LED_STATUS_RUN_ON,
  LED_STATUS_ERROR_ON,
  LED_STATUS_ERROR_OFF,
  LED_STATUS_UNKNOWN
} LedTaskCommand_t;

#define LED_QUEUE_LEN 1

void LED_Worker(void* pvParameters);
void LED_PutPacket(QueueHandle_t q, const LedTaskCommand_t *ptLedCmd);

#endif /* WORKERS_INC_LED_WORKER_H_ */
