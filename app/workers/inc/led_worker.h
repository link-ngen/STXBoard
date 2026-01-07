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

typedef enum {
  LED_CMD_CONFIGURING,  /* yellow and green LED alternate flashing */
  LED_CMD_CONFIGURED,   /* yellow glowing */
  LED_CMD_RUN_ON,       /* green  glowing */
  LED_CMD_ERROR_ON,     /* red glowing */
  LED_CMD_ERROR_OFF,    /* red off */
  LED_CMD_COUNT
} LED_COMMAND_E;

void LED_Worker(void* pvParameters);
bool LED_SendCommand(const void *ptLedCmd, TickType_t xTicksToWait);

#endif /* WORKERS_INC_LED_WORKER_H_ */
