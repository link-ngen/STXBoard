/**
 * NEOPXL Neopixel LED driver for STM32, Header
 * @Author: Minh Nguyen
 */

#ifndef __NEOPIXEL_H__
#define __NEOPIXEL_H__

#include "main.h"

extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_tim3_ch2;

#define  NEOPXL_NUM_LEDS	8

#define  NEOPXL_TIM_PRESCALE    0  // F_T3  = 64 MHz (11.90ns)
#define  NEOPXL_TIM_PERIODE   	104  // F_PWM = 800 kHz (1.25us)

#define  NEOPXL_LO_TIME        	37  // 29 * 13,9ns = 0.43us
#define  NEOPXL_HI_TIME        	69  // 58 * 13.9ns = 0.81us

//--------------------------------------------------------------
// RGB LED Farbdefinition (3 x 8bit)
//--------------------------------------------------------------
typedef struct {
  uint8_t red;    // 0...255 (als PWM-Wert)
  uint8_t green;  // 0...255 (als PWM-Wert)
  uint8_t blue;   // 0...255 (als PWM-Wert)
} __attribute__((__packed__)) NEOPXL_RGB_T;

//--------------------------------------------------------------
// HSV LED Farbdefinition
//--------------------------------------------------------------
typedef struct {
  uint16_t  hue;         // 0...359 (in Grad, 0=R, 120=G, 240=B)
  uint8_t   saturation;   // 0...100 (in Prozent)
  uint8_t   value;        // 0...100 (in Prozent)
} __attribute__((__packed__)) NEOPXL_HSV_T;

#define NEOPXL_TIM_BUFLEN	(NEOPXL_NUM_LEDS + 2) * 24

typedef struct NEOPXL_RESSOURCE_Ttag {
  TIM_HandleTypeDef *ptTim;
  uint32_t ulTimChannel;
  __IO uint8_t bDmaReady;
  uint16_t usaTimBuf[NEOPXL_TIM_BUFLEN];
  NEOPXL_RGB_T tLedBuf[NEOPXL_NUM_LEDS];
  void (*pfDmaCallback)(struct NEOPXL_RESSOURCE_Ttag *pNpxlRsc);
} NEOPXL_RESSOURCE_T;

// Library Interface
void Neopxl_Init(NEOPXL_RESSOURCE_T* ptNpxlRsc);
void Neopxl_Refresh(NEOPXL_RESSOURCE_T* ptNpxlRsc);
void Neopxl_Clear(NEOPXL_RESSOURCE_T* ptNpxlRsc);
void Neopxl_RGB2HSV(NEOPXL_HSV_T hsv_col, NEOPXL_RGB_T *rgb_col);
void Neopxl_One_RGB(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint32_t nr, NEOPXL_RGB_T rgb_col, uint8_t refresh);
void Neopxl_All_RGB(NEOPXL_RESSOURCE_T* ptNpxlRsc, NEOPXL_RGB_T rgb_col, uint8_t refresh);
void Neopxl_One_HSV(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint32_t nr, NEOPXL_HSV_T hsv_col, uint8_t refresh);
void Neopxl_All_HSV(NEOPXL_RESSOURCE_T* ptNpxlRsc, NEOPXL_HSV_T hsv_col, uint8_t refresh);
void Neopxl_Shift_Left(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh);
void Neopxl_Shift_Right(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh);
void Neopxl_Rotate_Left(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh);
void Neopxl_Rotate_Right(NEOPXL_RESSOURCE_T* ptNpxlRsc, uint8_t refresh);

#endif // __NEOPIXEL_H__
