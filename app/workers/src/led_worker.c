/*
 * led_worker.c
 *
 *  Created on: Dec 17, 2024
 *      Author: progr
 */


#include "led_worker.h"

void LED_Worker(void* pvParameters)
{
  LedRessource_t tLedRsc = { 0 };
  tLedRsc.xQueue = (QueueHandle_t*)pvParameters;

  while (1)
  {
    if(xQueueReceive(*tLedRsc.xQueue, &tLedRsc.tCmd, portMAX_DELAY) == pdPASS)
    {
      switch (tLedRsc.tCmd)
      {
      case LED_STATUS_CONFIG_BLINK:
        Led_ToggleConfigRun();
        vTaskDelay(500);
        break;
      case LED_STATUS_RUN_ON:
        Led_EnableConfigRun();
        break;
      case LED_STATUS_CONFIG_ERROR:
        Led_EnableConfigError();
        break;
      case LED_STATUS_ERROR:
      default:
        Led_EnableConfigError();
        Led_EnableError();
        break;
      }
    }
  }
}
