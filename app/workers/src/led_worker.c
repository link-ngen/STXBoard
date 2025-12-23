/*
 * led_worker.c
 *
 *  Created on: Dec 17, 2024
 *      Author: progr
 */

#include "led_worker.h"

typedef void (*LedCommandHandler_t)(void);

static eLedCommand s_eCmd;
static QueueHandle_t s_pxQueue;

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

static void Led_Configured(void)
{
  Led_EnableConfigured();
  Led_DisableError();
}

static void Led_ErrorOff(void)
{
  Led_DisableError();
}

static void Led_ErrorOn(void)
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
bool LED_SendCommand(const eLedCommand *ptLedCmd)
{
  // Validate input parameters
  if((ptLedCmd == NULL) || (*ptLedCmd >= LED_CMD_COUNT))
  {
    return false;
  }

  // ERROR_ON command has highest priority - always send and overwrite
  if(*ptLedCmd == LED_CMD_ERROR_ON)
  {
    eLedCommand tTmp = LED_CMD_ERROR_ON;
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
  eLedCommand tTmp = *ptLedCmd;
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
      [LED_CMD_CONFIGURING]   = Led_ConfigBlink,
      [LED_CMD_RUN_ON]        = Led_RunOn,
      [LED_CMD_CONFIGURED]    = Led_Configured,
      [LED_CMD_ERROR_OFF]     = Led_ErrorOff,
      [LED_CMD_ERROR_ON]      = Led_ErrorOn
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
