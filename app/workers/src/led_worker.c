/*
 * led_worker.c
 *
 *  Created on: Dec 17, 2024
 *      Author: progr
 */

#include "led_worker.h"

typedef void (*LedCommandHandler_t)(void);

static void Led_ConfigBlink(void)
{
  Led_ToggleConfigRun();
  Led_DisableError();
  vTaskDelay(pdMS_TO_TICKS(127));
}

static void Led_RunOn(void)
{
  Led_EnableConfigRun();
  Led_DisableError();
}

static void Led_ConfigError(void)
{
  Led_EnableConfigError();
  Led_DisableError();
}

static void Led_ErrorOff(void)
{
  Led_DisableError();
}

static void Led_ErrorOn(void)
{
  Led_EnableConfigError();
  Led_EnableError();
}

bool LED_SendCommand(QueueHandle_t q, const eLedCommand *ptLedCmd)
{
  if(NULL == q || NULL == ptLedCmd ||
    *ptLedCmd >= LED_CMD_COUNT)
    return false;

  eLedCommand tTmp = *ptLedCmd;
  xQueueOverwrite(q, &tTmp);
  return true;
}

void LED_Worker(void* pvParameters)
{
  LedCommandHandler_t ledCommandHandlers[] = {
      [LED_CMD_CONFIG_BLINK]   = Led_ConfigBlink,
      [LED_CMD_RUN_ON]         = Led_RunOn,
      [LED_CMD_CONFIG_ERROR]   = Led_ConfigError,
      [LED_CMD_ERROR_OFF]      = Led_ErrorOff,
      [LED_CMD_ERROR_ON]       = Led_ErrorOn
  };

  QueueHandle_t pxQueue = (QueueHandle_t)pvParameters;
  eLedCommand tCmd;

  while (1)
  {
    if(xQueueReceive(pxQueue, &tCmd, portMAX_DELAY) == pdPASS)
    {
      if(tCmd < LED_CMD_COUNT)
      {
        ledCommandHandlers[tCmd]();
      }
    }
  }
}
