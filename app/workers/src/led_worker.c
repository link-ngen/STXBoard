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
  vTaskDelay(127);
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

void LED_PutPacket(QueueHandle_t q, const LedTaskCommand_t *ptLedCmd)
{
  if(NULL == q || NULL == ptLedCmd)
    return;

  LedTaskCommand_t tTmp = *ptLedCmd;
  xQueueOverwrite(q, &tTmp);
}

void LED_Worker(void* pvParameters)
{
  LedCommandHandler_t ledCommandHandlers[] = {
      [LED_STATUS_CONFIG_BLINK]   = Led_ConfigBlink,
      [LED_STATUS_RUN_ON]         = Led_RunOn,
      [LED_STATUS_CONFIG_ERROR]   = Led_ConfigError,
      [LED_STATUS_ERROR_OFF]      = Led_ErrorOff,
      [LED_STATUS_ERROR_ON]       = Led_ErrorOn
  };

  QueueHandle_t pxQueue = (QueueHandle_t)pvParameters;
  LedTaskCommand_t tCmd;

  while (1)
  {
    if(xQueueReceive(pxQueue, &tCmd, portMAX_DELAY) == pdPASS)
    {
      if(tCmd < LED_STATUS_UNKNOWN)
      {
        ledCommandHandlers[tCmd]();
      }
    }
  }
}
