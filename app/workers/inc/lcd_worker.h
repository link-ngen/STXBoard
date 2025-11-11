/*
 * lcd_task.h
 *
 *  Created on: Dec 9, 2024
 *      Author: link
 */

#ifndef WORKERS_INC_LCD_WORKER_H_
#define WORKERS_INC_LCD_WORKER_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {
  LCD_COMMAND_IDLE_SCREEN,      /* screen if no activities        */
  LCD_COMMAND_BOOT_SCREEN,      /* screen after reset             */
  LCD_COMMAND_CONFIG_SCREEN,    /* screen during config           */
  LCD_COMMAND_IOXCHANGE_SCREEN, /* screen during IO data exchange */
  LCD_COMMAND_VERTEX_SCREEN,    /* screen like rotate octahedron  */
  LCD_COMMAND_ERROR_SCREEN,     /* screen error */
  LCD_COMMAND_UNKNOWN
} LcdCmdScreen_t;

typedef struct {
  LcdCmdScreen_t tCmd;
  uint16_t* usRotationAngle;
  char pcMessage[32];
} LcdPacket_t;

#define LCD_STATUS_QUEUE_LEN  1
#define LCD_DATA_QUEUE_LEN    5

void LCD_OctahedronWorker(void *pvParameters);
void LCD_Worker(void *pvParameters);
void LCD_PutPacket(QueueHandle_t q, const LcdPacket_t *ptLcdPkt);

#endif /* WORKERS_INC_LCD_WORKER_H_ */
