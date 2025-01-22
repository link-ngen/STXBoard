/*
 * lcd_task.c
 *
 *  Created on: Dec 9, 2024
 *      Author: link
 */

#include <stdio.h>
#include <math.h>

#include "lcd_worker.h"
#include "app_manager.h"
#include "ssd1306.h"
#include "OS_Dependent.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define PI_180 0.0174532

typedef void (*LCD_ScreenFunction_t)(void);

static const int8_t octahedron_vertex[6][3] = { { 0, 30, 0 }, { -20, 0, 0 },
    { 0, 0, 20 }, { 20, 0, 0 }, { 0, 0, -20 }, { 0, -30, 0 } };

static uint16_t wireoctahedron[6][2];

static const uint8_t originx = 64;
static const uint8_t originy = 32;

static void drawOctahedron(void)
{
  ssd1306_Line(wireoctahedron[0][0], wireoctahedron[0][1], wireoctahedron[1][0], wireoctahedron[1][1], White);
  ssd1306_Line(wireoctahedron[0][0], wireoctahedron[0][1], wireoctahedron[2][0], wireoctahedron[2][1], White);
  ssd1306_Line(wireoctahedron[0][0], wireoctahedron[0][1], wireoctahedron[3][0], wireoctahedron[3][1], White);
  ssd1306_Line(wireoctahedron[0][0], wireoctahedron[0][1], wireoctahedron[4][0], wireoctahedron[4][1], White);

  ssd1306_Line(wireoctahedron[1][0], wireoctahedron[1][1], wireoctahedron[2][0], wireoctahedron[2][1], White);
  ssd1306_Line(wireoctahedron[2][0], wireoctahedron[2][1], wireoctahedron[3][0], wireoctahedron[3][1], White);
  ssd1306_Line(wireoctahedron[3][0], wireoctahedron[3][1], wireoctahedron[4][0], wireoctahedron[4][1], White);
  ssd1306_Line(wireoctahedron[4][0], wireoctahedron[4][1], wireoctahedron[1][0], wireoctahedron[1][1], White);

  ssd1306_Line(wireoctahedron[5][0], wireoctahedron[5][1], wireoctahedron[1][0], wireoctahedron[1][1], White);
  ssd1306_Line(wireoctahedron[5][0], wireoctahedron[5][1], wireoctahedron[2][0], wireoctahedron[2][1], White);
  ssd1306_Line(wireoctahedron[5][0], wireoctahedron[5][1], wireoctahedron[3][0], wireoctahedron[3][1], White);
  ssd1306_Line(wireoctahedron[5][0], wireoctahedron[5][1], wireoctahedron[4][0], wireoctahedron[4][1], White);
}

void rotateOctahedron(int16_t pitch, int16_t roll, int16_t yaw)
{
  float rotx, roty, rotz, rotxx, rotyy, rotxxx, rotyyy;
  float pitchRad, rollRad, yawRad;
  pitchRad = pitch * PI_180;
  rollRad = roll * PI_180;
  yawRad = yaw * PI_180;

  for (int i = 0; i < 6; ++i)
  {
    //rotateY
    rotz = octahedron_vertex[i][2] * cos(pitchRad)
        - octahedron_vertex[i][0] * sin(pitchRad);
    rotx = octahedron_vertex[i][2] * sin(pitchRad)
        + octahedron_vertex[i][0] * cos(pitchRad);
    roty = octahedron_vertex[i][1];

    //rotateX
    rotyy = roty * cos(rollRad) - rotz * sin(rollRad);
    rotxx = rotx;

    //rotateZ
    rotxxx = rotxx * cos(yawRad) - rotyy * sin(yawRad);
    rotyyy = rotxx * sin(yawRad) + rotyy * cos(yawRad);

    //orthographic projection
    rotxxx = rotxxx + originx;
    rotyyy = rotyyy + originy;

    //store new vertices values for wireframe drawing
    wireoctahedron[i][0] = rotxxx;
    wireoctahedron[i][1] = rotyyy;
  }
}

void LCD_OctahedronWorker(void *pvParameters)
{
	/* setup */
  ssd1306_Init(); /* initialize the display */
  uint32_t stime, fps = 0, frames = 0;
  char string_fps[3];
  int16_t angle = 0;
  /* task loop */
  while(1)
  {
    ssd1306_Fill(Black);
    if(angle > 360)
    {
      angle = 0;
    }

    rotateOctahedron(angle, 0, 0);
    drawOctahedron();

    angle += 4;
    fps += 1000 / (OS_GetMilliSecCounter() - stime);
    stime = OS_GetMilliSecCounter();
    frames++;

    ssd1306_SetCursor(1, 55);
    sprintf(string_fps, "%d", (int)(fps / frames));
    ssd1306_WriteString(string_fps, Font_6x8, White);
    ssd1306_WriteString(" fps", Font_6x8, White);
    ssd1306_UpdateScreen();

    vTaskDelay(10);
  }
}

static void ShowIdleScreen(void)
{
  ssd1306_WriteString("Idle screen\n", Font_6x8, White);
}

static void ShowBootScreen(void)
{
  ssd1306_WriteString("Boot screen\n", Font_6x8, White);
}

static void ShowVertexScreen(void)
{
  ssd1306_WriteString("Vertex screen\n", Font_6x8, White);
}

static void ShowConfigScreen(void)
{
  ssd1306_WriteString("Config screen\n", Font_6x8, White);
}

static void ShowIoExchangeScreen(void)
{
  ssd1306_WriteString("IO Exchange screen\n", Font_6x8, White);
}

void LCD_Worker(void *pvParameters)
{
  LCD_ScreenFunction_t screenFunctions[] = {
      [LCD_COMMAND_IDLE_SCREEN]       = ShowIdleScreen,
      [LCD_COMMAND_BOOT_SCREEN]       = ShowBootScreen,
      [LCD_COMMAND_CONFIG_SCREEN]     = ShowConfigScreen,
      [LCD_COMMAND_VERTEX_SCREEN]     = ShowVertexScreen,
      [LCD_COMMAND_IOXCHANGE_SCREEN]  = ShowIoExchangeScreen
  };

  QueueHandle_t lcdQueue = (QueueHandle_t)pvParameters;
  LcdTaskScreen_t tLcdCmd;

  ssd1306_Init();

  while (1)
  {
    if(xQueueReceive(lcdQueue, &tLcdCmd, portMAX_DELAY) == pdPASS)
    {
      ssd1306_Fill(Black);
      if(tLcdCmd < LCD_COMMAND_UNKNOWN)
      {
        screenFunctions[tLcdCmd]();
      }
      else
      {
        ssd1306_WriteString("Unknown command received. Defaulting to Idle Screen.", Font_6x8, White);
        ShowIdleScreen();
      }
      ssd1306_SetCursor(0, 0);
      ssd1306_UpdateScreen();
      vTaskDelay(10);
    }
  }
}
