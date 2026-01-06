/*
 * led_worker.c
 *
 *  Created on: Dec 17, 2024
 *      Author: progr
 */

#include "led_worker.h"

typedef void (*LedCommandHandler_t)(void);

static LED_COMMAND_E s_eCmd;
static QueueHandle_t s_pxQueue;

static void prvConfigBlink(void)
{
  Led_ToggleConfigRun();
  Led_DisableError();
  vTaskDelay(pdMS_TO_TICKS(127));
}

static void prvRunOn(void)
{
  Led_EnableConfigRun();
  Led_DisableError();
}

static void prvConfigured(void)
{
  Led_EnableConfigured();
  Led_DisableError();
}

static void prvErrorOff(void)
{
  Led_DisableError();
}

static void prvErrorOn(void)
{
  Led_ToggleConfigRun();
  Led_EnableError();
  vTaskDelay(pdMS_TO_TICKS(512)); // ~1Hz
}

/**
 * @brief Sends a LED command to the queue
 * @param q Queue handle for LED commands
 * @param ptLedCmd Pointer to the LED command to send
 * @return true if command was sent successfully, false otherwise
 *
 * @note ERROR_ON command always has priority and overwrites any previous command
 * @note Duplicate commands (except ERROR_ON) are ignored to reduce queue traffic
 */
bool LED_SendCommand(const void *pvtLedCmd)
{
  LED_COMMAND_E *ptLedCmd = (LED_COMMAND_E*)pvtLedCmd;

  // Validate input parameters
  if((ptLedCmd == NULL) || (*ptLedCmd >= LED_CMD_COUNT))
  {
    return false;
  }

  // ERROR_ON command has highest priority - always send and overwrite
  if(*ptLedCmd == LED_CMD_ERROR_ON)
  {
    LED_COMMAND_E tTmp = LED_CMD_ERROR_ON;
    xQueueOverwrite(s_pxQueue, &tTmp);        // Force overwrite in queue
    s_eCmd = LED_CMD_ERROR_ON;       // Update global state
    return true;
  }

  // Avoid sending duplicate commands
  if(s_eCmd == *ptLedCmd)
  {
    return true;
  }

  // Send normal command to queue
  LED_COMMAND_E tTmp = *ptLedCmd;
  if(xQueueOverwrite(s_pxQueue, &tTmp) == pdPASS)
  {
    s_eCmd = *ptLedCmd;  // Update last sent command
    return true;
  }

  return false;  // Queue operation failed
}

void LED_Worker(void* pvParameters)
{
  LedCommandHandler_t ledCommandHandlers[] = {
      [LED_CMD_CONFIGURING]   = prvConfigBlink,
      [LED_CMD_RUN_ON]        = prvRunOn,
      [LED_CMD_CONFIGURED]    = prvConfigured,
      [LED_CMD_ERROR_OFF]     = prvErrorOff,
      [LED_CMD_ERROR_ON]      = prvErrorOn
  };

  s_pxQueue = (QueueHandle_t)pvParameters;

  while (1)
  {
    if(xQueueReceive(s_pxQueue, &s_eCmd, portMAX_DELAY) == pdPASS)
    {
      if(s_eCmd < LED_CMD_COUNT)
      {
        ledCommandHandlers[s_eCmd]();
      }
    }
  }
}
