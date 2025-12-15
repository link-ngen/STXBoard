/*
 * led_worker.h
 *
 *  Created on: Dec 17, 2024
 *      Author: progr
 */

#ifndef WORKERS_INC_LED_WORKER_H_
#define WORKERS_INC_LED_WORKER_H_

#include <stdbool.h>
#include "led_func.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef enum {
  LED_CMD_CONFIG_BLINK,
  LED_CMD_CONFIG_ERROR,
  LED_CMD_RUN_ON,
  LED_CMD_ERROR_ON,
  LED_CMD_ERROR_OFF,
  LED_CMD_COUNT
} eLedCommand;

#define LED_QUEUE_LEN 1

void LED_Worker(void* pvParameters);
bool LED_SendCommand(QueueHandle_t q, const eLedCommand *ptLedCmd);

#endif /* WORKERS_INC_LED_WORKER_H_ */
