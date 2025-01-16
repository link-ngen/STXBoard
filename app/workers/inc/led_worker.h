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
  LED_STATUS_ERROR,
} LedTaskCommand_t;

typedef struct LedRessource_tag
{
  /**< netx state function */
  QueueHandle_t *xQueue;
  LedTaskCommand_t tCmd;
} LedRessource_t;

void LED_Worker(void* pvParameters);

#endif /* WORKERS_INC_LED_WORKER_H_ */
