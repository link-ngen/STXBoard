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
static QueueHandle_t s_pxQueue;

typedef void (*NeopxlHandler)(NEOPXL_RESSOURCE_T *ptNpxlRsc);

/* Neopixel modus */
static void Neopxl_Continuous(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  typedef struct
  {
    NEOPXL_RGB_T tLastColor;
    bool fInitialized;
  } NEOPXL_CONT_STATE_T;

  static NEOPXL_CONT_STATE_T tPreviousState = { 0 };
  bool fUpdate = false;

  if (!tPreviousState.fInitialized ||
    tPreviousState.tLastColor.red != s_ptNeopxl->tColor.red ||
    tPreviousState.tLastColor.green != s_ptNeopxl->tColor.green ||
    tPreviousState.tLastColor.blue != s_ptNeopxl->tColor.blue)
  {
    fUpdate = true;
    tPreviousState.fInitialized = true;
    tPreviousState.tLastColor = s_ptNeopxl->tColor;
  }

  uint8_t fIsNeopixelOff = Neopxl_Are_Pixel_Off(ptNpxlRsc);
  if (fUpdate || (fIsNeopixelOff == 1))
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

static NEOPXL_RGB_T Neopxl_Wheel(uint8_t bWheelPos)
{
  static NEOPXL_RGB_T tRetColor = { 0 };
  if(bWheelPos < 85)
  {
    tRetColor.red = bWheelPos * 3;
    tRetColor.green = 255 - bWheelPos * 3;
    tRetColor.blue = 0;
  }
  else if(bWheelPos < 170)
  {
    bWheelPos -= 85;
    tRetColor.red = 255 - bWheelPos * 3;
    tRetColor.green = 0;
    tRetColor.blue = bWheelPos * 3;
  }
  else
  {
    bWheelPos -= 170;
    tRetColor.red = 0;
    tRetColor.green = bWheelPos * 3;
    tRetColor.blue = 255 - bWheelPos * 3;
  }
  return tRetColor;
}

static void Neopxl_Rainbow_Mode(NEOPXL_RESSOURCE_T *ptNpxlRsc)
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
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

bool Neopxl_UpdateData(const NEOPXL_DATA_ITEM_T *ptNeopxlData)
{
  if (ptNeopxlData == NULL)
  {
    return false;
  }

//  if (ptNeopxlData->eMode == s_ptNeopxl->eMode)
//  {
//    return true;
//  }

  NEOPXL_DATA_ITEM_T tTempData = *ptNeopxlData;
  if(xQueueOverwrite(s_pxQueue, &tTempData) == pdPASS)
  {
    return true;
  }

//  if (xQueueSend(s_pxQueue, (void*)ptNeopxlData, pdMS_TO_TICKS(10)) != pdPASS)
//  {
//    NEOPXL_DATA_ITEM_T tTempData;
//    xQueueReceive(s_pxQueue, (void*)&tTempData, 0);
//
//    return (xQueueSend(s_pxQueue, (void*)ptNeopxlData, 0) == pdPASS);
//  }
  return true;
}

void Neopxl_Worker(void *pvParameters)
{
  s_pxQueue = (QueueHandle_t)pvParameters;
  NEOPXL_DATA_ITEM_T tDataValue = { 0 };

  tDataValue.eMode       = NEOPXL_BLINKING_1_MODE;
  tDataValue.tColor      = (NEOPXL_RGB_T){ 0, NEOPXL_LOW_BRIGHTNESS, 0 };

  s_ptNeopxl = &tDataValue;

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

  NEOPXL_RESSOURCE_T tNeopxlRsc = { 0 };
  tNeopxlRsc.ptTim = &htim3;
  tNeopxlRsc.ulTimChannel = TIM_CHANNEL_2;

  Neopxl_Init(&tNeopxlRsc);

  while (1)
  {
    /* Check if we have new data from the PLC */
    if (xQueueReceive(s_pxQueue, &tDataValue, 0) == pdTRUE)
    {
      if (tDataValue.eMode < NEOPXL_UNKNOWN_MODE)
      {
        /* update new data  */
        s_ptNeopxl = &tDataValue;
      }
    }
    neopxlHandlers[s_ptNeopxl->eMode](&tNeopxlRsc);
  }
}
