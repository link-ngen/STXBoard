/*
 * lcd_task.h
 *
 *  Created on: Dec 9, 2024
 *      Author: link
 */

#ifndef WORKERS_INC_LCD_WORKER_H_
#define WORKERS_INC_LCD_WORKER_H_

typedef enum {
  LCD_COMMAND_IDLE_SCREEN,      /* screen if no activities */
  LCD_COMMAND_BOOT_SCREEN,      /* screen after reset */
  LCD_COMMAND_VERTEX_SCREEN,    /* screen like rotate octahedron */
  LCD_COMMAND_IOXCHANGE_SCREEN, /* screen during IO data exchange */
  LCD_COMMAND_UNKNOWN
} LcdTaskScreen_t;

#define LCD_QUEUE_LEN 2

void LCD_OctahedronWorker(void *pvParameters);
void LCD_Worker(void *pvParameters);

#endif /* WORKERS_INC_LCD_WORKER_H_ */
