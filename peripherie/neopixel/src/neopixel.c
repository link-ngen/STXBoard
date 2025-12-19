/**
 * Neopixel LED driver for STM32
 * Based upon the WS2812 Library for STM32F4 from Uwe Becker, http://mikrocontroller.bplaced.net/wordpress/?page_id=3665
 *
 * @Author: Nicolas Dammin, 2016
 */
#include <string.h>
#include "neopixel.h"

static NEOPXL_RESSOURCE_T *s_ptNeopxl;
static uint16_t neopixel_lut[256][8];

static void Neopxl_InitLUT(void)
{
  for(int val = 0; val < 256; ++val)
  {
    for(int bit = 7; bit >= 0; --bit)
    {
      neopixel_lut[val][7 - bit] = (val & (1 << bit)) ? NEOPXL_HI_TIME : NEOPXL_LO_TIME;
    }
  }
}

static void DMA_Callback(NEOPXL_RESSOURCE_T* ptNpxlRsc)
{
  ptNpxlRsc->fDmaReady = 1;
}

/**
 * Internal function, calculates the HI or LO values for the 800 kHz WS2812 signal and puts them into a buffer for the Timer-DMA
 *
 */
static void calcBuf(NEOPXL_RESSOURCE_T* ptNpxlRsc)
{
  uint32_t pos = 0;
  for(uint32_t n = 0; n < NEOPXL_NUM_LEDS; ++n)
  {
    NEOPXL_RGB_T led = ptNpxlRsc->tLedBuf[n];
    //  G, R, B
    const uint16_t *lut;

    lut = neopixel_lut[led.green];
    memcpy(&ptNpxlRsc->usaTimBuf[pos], lut, sizeof(uint16_t) * 8);
    pos += 8;

    lut = neopixel_lut[led.red];
    memcpy(&ptNpxlRsc->usaTimBuf[pos], lut, sizeof(uint16_t) * 8);
    pos += 8;

    lut = neopixel_lut[led.blue];
    memcpy(&ptNpxlRsc->usaTimBuf[pos], lut, sizeof(uint16_t) * 8);
    pos += 8;
  }

  // Reset-Latch (mind. 50 µs = ca. 48 Pulse bei 800 kHz)
  for(uint32_t i = 0; i < 48; ++i)
    ptNpxlRsc->usaTimBuf[pos++] = 0;
}

/**
 * Internal function; start DMA transfer
 */
static void startDMA(NEOPXL_RESSOURCE_T* ptNpxlRsc)
{
  ptNpxlRsc->fDmaReady = 0;
	HAL_TIM_PWM_Start_DMA(ptNpxlRsc->ptTim, ptNpxlRsc->ulTimChannel, (uint32_t *)ptNpxlRsc->usaTimBuf, NEOPXL_TIM_BUFLEN);
}

void Neopxl_Init(NEOPXL_RESSOURCE_T* ptNpxlRsc)
{
  Neopxl_InitLUT();
  s_ptNeopxl = ptNpxlRsc;
  ptNpxlRsc->fDmaReady = 1;
}

void Neopxl_Refresh(NEOPXL_RESSOURCE_T* ptNpxlRsc)
{
	while(!ptNpxlRsc->fDmaReady);
	calcBuf(ptNpxlRsc);
	startDMA(ptNpxlRsc);
}

/**
 * Set all LEDs to 0 (off) and update
 */
void Neopxl_Clear(NEOPXL_RESSOURCE_T* ptNpxlRsc)
{
	for(uint16_t num = 0; num < NEOPXL_NUM_LEDS; ++num)
	{
	  ptNpxlRsc->tLedBuf[num] = (NEOPXL_RGB_T){0,0,0};
	}
	Neopxl_Refresh(ptNpxlRsc);
}

/**
 * Convert HSV-Value to RGB Value for WS2812 LEDs
 * (from www.ulrichradig.de)
 */
void Neopxl_RGB2HSV(NEOPXL_HSV_T hsv_col, NEOPXL_RGB_T *rgb_col)
{
  uint8_t diff;

  // Grenzwerte
  if(hsv_col.hue>359) hsv_col.hue=359;
  if(hsv_col.saturation>100) hsv_col.saturation=100;
  if(hsv_col.value>100) hsv_col.value=100;

  if(hsv_col.hue < 61) {
    rgb_col->red = 255;
    rgb_col->green = (425 * hsv_col.hue) / 100;
    rgb_col->blue = 0;
  }else if(hsv_col.hue < 121){
    rgb_col->red = 255 - ((425 * (hsv_col.hue-60))/100);
    rgb_col->green = 255;
    rgb_col->blue = 0;
  }else if(hsv_col.hue < 181){
    rgb_col->red = 0;
    rgb_col->green = 255;
    rgb_col->blue = (425 * (hsv_col.hue-120))/100;
  }else if(hsv_col.hue < 241){
    rgb_col->red = 0;
    rgb_col->green = 255 - ((425 * (hsv_col.hue-180))/100);
    rgb_col->blue = 255;
  }else if(hsv_col.hue < 301){
    rgb_col->red = (425 * (hsv_col.hue-240))/100;
    rgb_col->green = 0;
    rgb_col->blue = 255;
  }else {
    rgb_col->red = 255;
    rgb_col->green = 0;
    rgb_col->blue = 255 - ((425 * (hsv_col.hue-300))/100);
  }

  hsv_col.saturation = 100 - hsv_col.saturation;
  diff = ((255 - rgb_col->red) * hsv_col.saturation)/100;
  rgb_col->red = rgb_col->red + diff;
  diff = ((255 - rgb_col->green) * hsv_col.saturation)/100;
  rgb_col->green = rgb_col->green + diff;
  diff = ((255 - rgb_col->blue) * hsv_col.saturation)/100;
  rgb_col->blue = rgb_col->blue + diff;

  rgb_col->red = (rgb_col->red * hsv_col.value)/100;
  rgb_col->green = (rgb_col->green * hsv_col.value)/100;
  rgb_col->blue = (rgb_col->blue * hsv_col.value)/100;
}

/**
 * Set one LED (R, G, B values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void Neopxl_One_RGB(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint32_t nr, NEOPXL_RGB_T rgb_col, uint8_t refresh)
{
  if(nr < NEOPXL_NUM_LEDS)
  {
    ptNpxlRsc->tLedBuf[nr] = rgb_col;
	  if (refresh)
	    Neopxl_Refresh(ptNpxlRsc);
  }
}

/**
 * Set all LEDs (R, G, B values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void Neopxl_All_RGB(NEOPXL_RESSOURCE_T* ptNpxlRsc, NEOPXL_RGB_T rgb_col, uint8_t refresh)
{
  for(uint32_t n = 0; n < NEOPXL_NUM_LEDS; ++n)
    ptNpxlRsc->tLedBuf[n] = rgb_col;

  if (refresh)
    Neopxl_Refresh(ptNpxlRsc);
}

/**
 * Set one LED (H, S, V values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void Neopxl_One_HSV(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint32_t nr, NEOPXL_HSV_T hsv_col, uint8_t refresh)
{
  NEOPXL_RGB_T rgb_col;
  if(nr < NEOPXL_NUM_LEDS)
  {
    // convert to RGB
    Neopxl_RGB2HSV(hsv_col, &rgb_col);
    ptNpxlRsc->tLedBuf[nr] = rgb_col;

    if(refresh)
      Neopxl_Refresh(ptNpxlRsc);
  }
}

/**
 * Set all LEDs (H, S, V values). If refresh == 1, update LEDs, otherwise just update buffer (if several function calls are to be done before refresh)
 */
void Neopxl_All_HSV(NEOPXL_RESSOURCE_T* ptNpxlRsc, NEOPXL_HSV_T hsv_col, uint8_t refresh)
{
  NEOPXL_RGB_T rgb_col;

  // convert to RGB
  Neopxl_RGB2HSV(hsv_col, &rgb_col);
  for(uint32_t n = 0; n < NEOPXL_NUM_LEDS; ++n)
  {
    ptNpxlRsc->tLedBuf[n] = rgb_col;
  }
  if (refresh)
    Neopxl_Refresh(ptNpxlRsc);
}

/**
 * Shift all LED values one to the left. Last one will be turned off
 */
void Neopxl_Shift_Left(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh)
{
  uint32_t n;

  if(NEOPXL_NUM_LEDS > 1)
  {
    for(n = 1; n < NEOPXL_NUM_LEDS; ++n)
    {
      ptNpxlRsc->tLedBuf[n-1] = ptNpxlRsc->tLedBuf[n];
    }
    ptNpxlRsc->tLedBuf[n-1] = (NEOPXL_RGB_T){0,0,0};

    if(refresh)
      Neopxl_Refresh(ptNpxlRsc);
  }
}

/**
 * Shift all LED values one to the right. First one will be turned off
 */
void Neopxl_Shift_Right(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh)
{
  uint32_t n;

  if(NEOPXL_NUM_LEDS > 1)
  {
    for(n = NEOPXL_NUM_LEDS - 1; n > 0; --n)
    {
      ptNpxlRsc->tLedBuf[n] = ptNpxlRsc->tLedBuf[n-1];
    }
    ptNpxlRsc->tLedBuf[n] = (NEOPXL_RGB_T){0,0,0};

    if(refresh)
      Neopxl_Refresh(ptNpxlRsc);
  }
}

/**
 * Shift all LED values one to the left. Last LED value will be the previous first value
 */
void Neopxl_Rotate_Left(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh)
{
  uint32_t n;
  NEOPXL_RGB_T led;

  if(NEOPXL_NUM_LEDS > 1)
  {
    led = ptNpxlRsc->tLedBuf[0];
    for(n = 1; n < NEOPXL_NUM_LEDS; ++n)
    {
      ptNpxlRsc->tLedBuf[n-1] = ptNpxlRsc->tLedBuf[n];
    }
    ptNpxlRsc->tLedBuf[n-1] = led;

    if(refresh)
      Neopxl_Refresh(ptNpxlRsc);
  }
}

/**
 * Shift all LED values one to the right. First LED value will be the previous last value
 */
void Neopxl_Rotate_Right(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh)
{
  uint32_t n;
  NEOPXL_RGB_T led;

  if(NEOPXL_NUM_LEDS > 1)
  {
    led = ptNpxlRsc->tLedBuf[NEOPXL_NUM_LEDS - 1];
    for(n = NEOPXL_NUM_LEDS - 1; n > 0; --n)
    {
      ptNpxlRsc->tLedBuf[n] = ptNpxlRsc->tLedBuf[n-1];
    }
    ptNpxlRsc->tLedBuf[n] = led;

    if(refresh)
      Neopxl_Refresh(ptNpxlRsc);
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_2);
  DMA_Callback(s_ptNeopxl);
}
