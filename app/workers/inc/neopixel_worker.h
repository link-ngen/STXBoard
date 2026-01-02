/*
 * neopixel_worker.h
 *
 *  Created on: 19.12.2025
 *      Author: nnguyen
 */

#ifndef WORKERS_INC_NEOPIXEL_WORKER_H_
#define WORKERS_INC_NEOPIXEL_WORKER_H_

#include <stdbool.h>
#include "neopixel.h"

typedef enum
{
  NEOPXL_CONTINUOUS_MODE = 0x00,         /* neopixel stay lighted with given color or default color */
  NEOPXL_BLINKING_1_MODE = 0x01,         /* Continuous blinking 1Hz with given color */
  NEOPXL_BLINKING_2_MODE = 0x02,         /* Continuous blinking 2Hz with given color */
  NEOPXL_BLINKING_3_MODE = 0x03,         /* Continuous blinking 3Hz with given color */
  NEOPXL_FLASHING_1_MODE = 0x04,         /* Flashing with given color (100 ms on → 2s off)*/
  NEOPXL_FLASHING_2_MODE = 0x05,         /* Flashing with given color (100 ms on → 200ms off → 100ms on → 1s off) */
  NEOPXL_FLASHING_3_MODE = 0x06,         /* Flashing with given color (100 ms on → 200ms off → 100ms on → 200ms off → 100ms on → 1s off) */
  NEOPXL_RAINBOW_MODE = 0x07,            /* Rainbow rotating */
  NEOPXL_UNKNOWN_MODE
} NEOPXL_MODE_E;

typedef struct NEOPXL_DATA_ITEM_Ttag
{
  NEOPXL_MODE_E eMode;              /* 1 Byte */
  NEOPXL_RGB_T tColor;              /* 3 Byte */
} NEOPXL_DATA_ITEM_T;


/* ================= PUBLIC API ================= */
void Neopxl_Worker(void *pvParameters); /* params queue and command */
bool Neopxl_UpdateData(const NEOPXL_DATA_ITEM_T *ptNeopxlData);

#endif /* WORKERS_INC_NEOPIXEL_WORKER_H_ */
