/*
 * led_worker.c
 *
 *  Created on: Dec 17, 2024
 *      Author: progr
 */


#include "led_worker.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef void (*LedCommandHandler_t)(void);

static void Led_HandleConfigBlink(void)
{
  Led_ToggleConfigRun();
  Led_DisableError();
  vTaskDelay(127);
}

static void Led_HandleRunOn(void)
{
  Led_EnableConfigRun();
  Led_DisableError();
}

static void Led_HandleConfigError(void)
{
  Led_EnableConfigError();
  Led_DisableError();
}

static void Led_HandleErrorOff(void)
{
  Led_DisableError();
}

static void Led_HandleErrorOn(void)
{
  Led_EnableConfigError();
  Led_EnableError();
}

void LED_Worker(void* pvParameters)
{
  LedCommandHandler_t ledCommandHandlers[] = {
      [LED_STATUS_CONFIG_BLINK]   = Led_HandleConfigBlink,
      [LED_STATUS_RUN_ON]         = Led_HandleRunOn,
      [LED_STATUS_CONFIG_ERROR]   = Led_HandleConfigError,
      [LED_STATUS_ERROR_OFF]      = Led_HandleErrorOff,
      [LED_STATUS_ERROR_ON]       = Led_HandleErrorOn
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
