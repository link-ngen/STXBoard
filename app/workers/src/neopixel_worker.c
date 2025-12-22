/*
 * neopixel_worker.c
 *
 *  Created on: 19.12.2025
 *      Author: nnguyen
 */
#include <stdbool.h>
#include "neopixel_worker.h"

#include "tim.h"
#include "FreeRTOS.h"
#include "queue.h"

static NEOPXL_DATA_ITEM_T *s_ptNeopxl;

typedef void (*NeopxlHandler)(NEOPXL_RESSOURCE_T *ptNpxlRsc);

static bool isValidBrightness(uint8_t brightness)
{
  switch (brightness)
  {
  case NEOPXL_LOW_BRIGHTNESS:
  case NEOPXL_MIDDLE_BRIGHTNESS:
  case NEOPXL_HIGH_BRIGHTNESS:
  case NEOPXL_MAX_BRIGHTNESS:
    return true;
  default:
    return false;
  }
}

/* Neopixel modus */
static void Neopxl_Continuous(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  typedef struct
  {
    NEOPXL_RGB_T tLastColor;
    NEOPXL_BRIGHTNESS_E eLastBrightness;
    bool bInitialized;
  } NEOPXL_CONT_STATE_T;

  static NEOPXL_CONT_STATE_T tPreviousState = { 0 };
  bool fUpdate = false;

  if (!tPreviousState.bInitialized ||
    tPreviousState.tLastColor.red != s_ptNeopxl->tColor.red ||
    tPreviousState.tLastColor.green != s_ptNeopxl->tColor.green ||
    tPreviousState.tLastColor.blue != s_ptNeopxl->tColor.blue ||
    tPreviousState.eLastBrightness != s_ptNeopxl->eBrightness)
  {
    fUpdate = true;
    tPreviousState.bInitialized = true;
    tPreviousState.eLastBrightness = s_ptNeopxl->eBrightness;
    tPreviousState.tLastColor = s_ptNeopxl->tColor;
  }

  if (fUpdate)
  {
    Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  }
  vTaskDelay(pdMS_TO_TICKS(1));
}

static void Neopxl_Blinking_Mode_1(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(500));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(500));
}

static void Neopxl_Blinking_Mode_2(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(250));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(250));
}

static void Neopxl_Blinking_Mode_3(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(167));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(167));
}

static void Neopxl_Flashing_Mode_1(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(100));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(2000));
}

static void Neopxl_Flashing_Mode_2(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(100));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(200));

  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(100));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(1000));
}

static void Neopxl_Flashing_Mode_3(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(100));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(200));

  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(100));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(200));

  Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  vTaskDelay(pdMS_TO_TICKS(100));

  Neopxl_Clear(ptNpxlRsc);
  vTaskDelay(pdMS_TO_TICKS(1000));
}

static NEOPXL_RGB_T Neopxl_Wheel(uint8_t wheel_pos)
{
  static NEOPXL_RGB_T tRetColor = { 0 };
  if(wheel_pos < 85)
  {
    tRetColor.red = wheel_pos * 3;
    tRetColor.green = 255 - wheel_pos * 3;
    tRetColor.blue = 0;
  }
  else if(wheel_pos < 170)
  {
    wheel_pos -= 85;
    tRetColor.red = 255 - wheel_pos * 3;
    tRetColor.green = 0;
    tRetColor.blue = wheel_pos * 3;
  }
  else
  {
    wheel_pos -= 170;
    tRetColor.red = 0;
    tRetColor.green = wheel_pos * 3;
    tRetColor.blue = 255 - wheel_pos * 3;
  }
  return tRetColor;
}


void Neopxl_Rainbow_Mode(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  NEOPXL_RGB_T tNeopxl;
  for (uint16_t j = 0; j < 256 * 5; ++j) // 5 cycles of all colors on wheel
  {
    for (uint16_t i = 0; i < NEOPXL_NUM_LEDS; ++i)
    {
      tNeopxl = Neopxl_Wheel(((i * 256 / NEOPXL_NUM_LEDS) + j) & 255);
      Neopxl_One_RGB(ptNpxlRsc, i, tNeopxl, 0);
    }
    Neopxl_Refresh(ptNpxlRsc);
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void Neopxl_Worker(void *pvParameters)
{
  NEOPXL_DATA_ITEM_T tDataValue = { 0 };
  tDataValue.eMode       = NEOPXL_BLINKING_1_MODE;
  tDataValue.eBrightness = NEOPXL_LOW_BRIGHTNESS;
  tDataValue.tColor      = (NEOPXL_RGB_T){ 0, tDataValue.eBrightness, 0 };

  s_ptNeopxl = &tDataValue;

  NEOPXL_RESSOURCE_T tNeopxlRsc = { 0 };
  tNeopxlRsc.ptTim = &htim3;
  tNeopxlRsc.ulTimChannel = TIM_CHANNEL_2;

  QueueHandle_t pxQueue = (QueueHandle_t)pvParameters;

  NeopxlHandler neopxlHandlers[] = {
    [NEOPXL_CONTINUOUS_MODE] = Neopxl_Continuous,
    [NEOPXL_BLINKING_1_MODE] = Neopxl_Blinking_Mode_1,
    [NEOPXL_BLINKING_2_MODE] = Neopxl_Blinking_Mode_2,
    [NEOPXL_BLINKING_3_MODE] = Neopxl_Blinking_Mode_3,
    [NEOPXL_FLASHING_1_MODE] = Neopxl_Flashing_Mode_1,
    [NEOPXL_FLASHING_2_MODE] = Neopxl_Flashing_Mode_2,
    [NEOPXL_FLASHING_3_MODE] = Neopxl_Flashing_Mode_3,
    [NEOPXL_RAINBOW_ROTATING_MODE] = Neopxl_Rainbow_Mode
  };

  Neopxl_Init(&tNeopxlRsc);

  while (1)
  {
    /* Check if we have new data from the PLC */
    if (xQueueReceive(pxQueue, &tDataValue, 0) == pdTRUE)
    {
      if (tDataValue.eMode < NEOPXL_UNKNOWN_MODE)
      {
        /* update new data  */
        s_ptNeopxl = &tDataValue;
        s_ptNeopxl->eBrightness = isValidBrightness(tDataValue.eBrightness) ? tDataValue.eBrightness : NEOPXL_LOW_BRIGHTNESS;
      }
    }
    neopxlHandlers[s_ptNeopxl->eMode](&tNeopxlRsc);
  }
}
