/*
 * lcd_task.h
 *
 *  Created on: Dec 9, 2024
 *      Author: link
 */

#ifndef WORKERS_INC_LCD_WORKER_H_
#define WORKERS_INC_LCD_WORKER_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {
  LCD_IDLE_SCREEN,      /* screen if no activities        */
  LCD_BOOT_SCREEN,      /* screen after reset             */
  LCD_CONFIG_SCREEN,    /* screen during config           */
  LCD_IOXCHANGE_SCREEN, /* screen during IO data exchange */
  LCD_VERTEX_SCREEN,    /* screen like rotate octahedron  */
  LCD_ERROR_SCREEN,     /* screen error */
  LCD_SCREEN_COUNT
} eLcdScreen;

typedef struct {
  eLcdScreen eScreen;
  char pcMessage[32];
  void* pvCustomData;
} LcdCommand_t;

#define LCD_STATUS_QUEUE_LEN  1
#define LCD_DATA_QUEUE_LEN    5

void LCD_OctahedronWorker(void *pvParameters);
void LCD_Worker(void *pvParameters);
bool LCD_SendCommand(QueueHandle_t q, const LcdCommand_t *ptCommand);

#endif /* WORKERS_INC_LCD_WORKER_H_ */
