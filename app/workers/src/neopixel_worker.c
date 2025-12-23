/*
 * neopixel_worker.c
 *
 *  Created on: 19.12.2025
 *      Author: nnguyen
 */
#include <string.h>
#include "neopixel_worker.h"

#include "tim.h"
#include "FreeRTOS.h"
#include "queue.h"

typedef struct
{
  union
  {
    /* Rainbow Mode */
    struct
    {
      uint16_t colorIndex;
      uint16_t ledIndex;
      uint8_t cycles;
    } tRainbow;

    /* Blinking/Flashing mode */
    struct
    {
      bool bOn;             // current on/off
      uint8_t flashCount;   // num of flash
      uint8_t maxFlashes;
    } tBlink;

    /* Continuous Mode Cache */
    struct
    {
      NEOPXL_RGB_T lastColor;
      bool initialized;
    } tContinuous;
  } modus;

  uint32_t ulLastUpdate;
  uint32_t ulPhaseDuration;
  NEOPXL_MODE_E eCurrentMode;
} NEOPXL_MODE_STATE_T;

static NEOPXL_MODE_STATE_T s_tModeState;
static NEOPXL_DATA_ITEM_T *s_ptNeopxl;
static QueueHandle_t s_pxQueue;

typedef void (*NeopxlHandler)(NEOPXL_RESSOURCE_T *ptNpxlRsc);

/**
 * Color wheel function
 */
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

/* Initializes state for a new mode */
static void Neopxl_InitializeMode(NEOPXL_MODE_E eNewMode)
{
  memset(&s_tModeState, 0, sizeof(s_tModeState));
  s_tModeState.eCurrentMode = eNewMode;
  s_tModeState.ulLastUpdate = xTaskGetTickCount();

  switch (eNewMode)
  {
  case NEOPXL_CONTINUOUS_MODE:
    s_tModeState.modus.tContinuous.initialized = false;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(1000);
    break;

  case NEOPXL_BLINKING_1_MODE:
    s_tModeState.modus.tBlink.bOn = true;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(500);
    break;

  case NEOPXL_BLINKING_2_MODE:
    s_tModeState.modus.tBlink.bOn = true;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(250);
    break;

  case NEOPXL_BLINKING_3_MODE:
    s_tModeState.modus.tBlink.bOn = true;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(167);
    break;

  case NEOPXL_FLASHING_1_MODE:
    s_tModeState.modus.tBlink.bOn = true;
    s_tModeState.modus.tBlink.maxFlashes = 1;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(100);
    break;

  case NEOPXL_FLASHING_2_MODE:
    s_tModeState.modus.tBlink.bOn = true;
    s_tModeState.modus.tBlink.flashCount = 0;
    s_tModeState.modus.tBlink.maxFlashes = 2;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(100);
    break;

  case NEOPXL_FLASHING_3_MODE:
    s_tModeState.modus.tBlink.bOn = true;
    s_tModeState.modus.tBlink.flashCount = 0;
    s_tModeState.modus.tBlink.maxFlashes = 3;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(100);
    break;

  case NEOPXL_RAINBOW_MODE:
    s_tModeState.modus.tRainbow.colorIndex = 0;
    s_tModeState.modus.tRainbow.ledIndex = 0;
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(50);
    break;

  default:
    s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(10);
    break;
  }
}

/* Checks whether update time has been reached */
static bool Neopxl_IsTimeForUpdate(void)
{
  uint32_t ulCurrentTime = xTaskGetTickCount();
  return ((ulCurrentTime - s_tModeState.ulLastUpdate) >= s_tModeState.ulPhaseDuration);
}

/* Neopixel modus */
static void Neopxl_Continuous(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  if(!s_tModeState.modus.tContinuous.initialized ||
      s_tModeState.modus.tContinuous.lastColor.red != s_ptNeopxl->tColor.red ||
      s_tModeState.modus.tContinuous.lastColor.green != s_ptNeopxl->tColor.green ||
      s_tModeState.modus.tContinuous.lastColor.blue != s_ptNeopxl->tColor.blue)
  {
    Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);

    s_tModeState.modus.tContinuous.initialized = true;
    s_tModeState.modus.tContinuous.lastColor = s_ptNeopxl->tColor;
  }

  s_tModeState.ulLastUpdate = xTaskGetTickCount();
  s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(1000); // 1s Polling
}

/* Generic Blinking Handler */
static void Neopxl_Generic_Blink(NEOPXL_RESSOURCE_T *ptNpxlRsc, uint32_t ulOnTime, uint32_t ulOffTime)
{
    s_tModeState.ulLastUpdate = xTaskGetTickCount();

    if (s_tModeState.modus.tBlink.bOn)
    {
        Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
        s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(ulOnTime);
    }
    else
    {
        Neopxl_Clear(ptNpxlRsc);
        s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(ulOffTime);
    }

    s_tModeState.modus.tBlink.bOn = !s_tModeState.modus.tBlink.bOn;
}

static void Neopxl_Blinking_Mode_1(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_Generic_Blink(ptNpxlRsc, 500, 500);
}

static void Neopxl_Blinking_Mode_2(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_Generic_Blink(ptNpxlRsc, 250, 250);
}

static void Neopxl_Blinking_Mode_3(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_Generic_Blink(ptNpxlRsc, 167, 167);
}

/* Generic Flashing Handler */
static void Neopxl_Generic_Flash(NEOPXL_RESSOURCE_T *ptNpxlRsc, uint8_t bFlashPattern)
{
  s_tModeState.ulLastUpdate = xTaskGetTickCount();

  static const uint16_t ausPatterns[][6] = {
    // {ON, PAUSE} of FLASHING_1_MODE
    { 100, 2000, 0, 0, 0, 0 },
    // {ON1, PAUSE1, ON2, PAUSE2} of FLASHING_2_MODE
    { 100, 200, 100, 1000, 0, 0 },
    // {ON1, PAUSE1, ON2, PAUSE2, ON3, PAUSE3} of FLASHING_3_MODE
    { 100, 200, 100, 200, 100, 1000 } };

  const uint16_t *pusPattern = ausPatterns[bFlashPattern];

  uint8_t currentPhase = s_tModeState.modus.tBlink.flashCount;
  uint8_t maxPhases = (bFlashPattern + 1) * 2;

  if(currentPhase >= maxPhases)
  {
    currentPhase = 0;
    s_tModeState.modus.tBlink.flashCount = 0;
  }

  if((currentPhase & 1) == 0) /* modulo 2 */
  {
    // ON-Phase
    Neopxl_All_RGB(ptNpxlRsc, s_ptNeopxl->tColor, 1);
  }
  else
  {
    // OFF-Phase
    Neopxl_Clear(ptNpxlRsc);
  }

  s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(pusPattern[currentPhase]);
  s_tModeState.modus.tBlink.flashCount++;
}

static void Neopxl_Flashing_Mode_1(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_Generic_Flash(ptNpxlRsc, 0);
}

static void Neopxl_Flashing_Mode_2(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_Generic_Flash(ptNpxlRsc, 1);
}

static void Neopxl_Flashing_Mode_3(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  Neopxl_Generic_Flash(ptNpxlRsc, 2);
}

static void Neopxl_Rainbow_Mode(NEOPXL_RESSOURCE_T *ptNpxlRsc)
{
  s_tModeState.ulLastUpdate = xTaskGetTickCount();

  NEOPXL_RGB_T tNeopxl = Neopxl_Wheel(((s_tModeState.modus.tRainbow.ledIndex * 256 / NEOPXL_NUM_LEDS) + s_tModeState.modus.tRainbow.colorIndex) & 255);

  Neopxl_One_RGB(ptNpxlRsc, s_tModeState.modus.tRainbow.ledIndex, tNeopxl, 0);
  s_tModeState.modus.tRainbow.ledIndex++;

  if(s_tModeState.modus.tRainbow.ledIndex >= NEOPXL_NUM_LEDS)
  {
    s_tModeState.modus.tRainbow.ledIndex = 0;
    Neopxl_Refresh(ptNpxlRsc);
    s_tModeState.modus.tRainbow.colorIndex++;

    if(s_tModeState.modus.tRainbow.colorIndex >= 256 * 5)
    {
      s_tModeState.modus.tRainbow.colorIndex = 0;
      s_tModeState.modus.tRainbow.cycles++;
    }
  }

  s_tModeState.ulPhaseDuration = pdMS_TO_TICKS(50);
}

bool Neopxl_UpdateData(const NEOPXL_DATA_ITEM_T *ptNeopxlData)
{
  if (ptNeopxlData == NULL ||
      ptNeopxlData->eMode >= NEOPXL_UNKNOWN_MODE)
  {
    return false;
  }
  NEOPXL_DATA_ITEM_T tTempData = *ptNeopxlData;
  return (xQueueOverwrite(s_pxQueue, &tTempData) == pdPASS);
}

void Neopxl_Worker(void *pvParameters)
{
  s_pxQueue = (QueueHandle_t)pvParameters;

  NEOPXL_DATA_ITEM_T tNewData;
  NEOPXL_DATA_ITEM_T tCurrentData = { .eMode = NEOPXL_BLINKING_1_MODE,
                                      .tColor = { 0, 0, NEOPXL_LOW_BRIGHTNESS } };
  s_ptNeopxl = &tCurrentData;

  NeopxlHandler neopxlHandlers[] = {
    [NEOPXL_CONTINUOUS_MODE] = Neopxl_Continuous,
    [NEOPXL_BLINKING_1_MODE] = Neopxl_Blinking_Mode_1,
    [NEOPXL_BLINKING_2_MODE] = Neopxl_Blinking_Mode_2,
    [NEOPXL_BLINKING_3_MODE] = Neopxl_Blinking_Mode_3,
    [NEOPXL_FLASHING_1_MODE] = Neopxl_Flashing_Mode_1,
    [NEOPXL_FLASHING_2_MODE] = Neopxl_Flashing_Mode_2,
    [NEOPXL_FLASHING_3_MODE] = Neopxl_Flashing_Mode_3,
    [NEOPXL_RAINBOW_MODE]    = Neopxl_Rainbow_Mode
  };

  NEOPXL_RESSOURCE_T tNeopxlRsc = { 0 };
  tNeopxlRsc.ptTim = &htim3;
  tNeopxlRsc.ulTimChannel = TIM_CHANNEL_2;

  Neopxl_Init(&tNeopxlRsc);
  Neopxl_InitializeMode(tCurrentData.eMode);

  while(1)
  {
    /* Check if we have new data from the PLC */
    if(xQueueReceive(s_pxQueue, &tNewData, 0) == pdTRUE)
    {
      if(tNewData.eMode < NEOPXL_UNKNOWN_MODE)
      {
        // change state?
        if(tCurrentData.eMode != tNewData.eMode)
        {
          Neopxl_Clear(&tNeopxlRsc);
          Neopxl_InitializeMode(tNewData.eMode);
        }

        tCurrentData = tNewData;
        s_ptNeopxl = &tCurrentData;
      }
    }

    // check if it's time for update
    if(Neopxl_IsTimeForUpdate())
    {
      neopxlHandlers[s_ptNeopxl->eMode](&tNeopxlRsc);
    }

    // delay for cpu schedule
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
