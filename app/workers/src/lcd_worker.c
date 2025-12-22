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
#include "ssd1306_gfx.h"
#include "OS_Dependent.h"

#include "cat_frames.h"

#define PI_180      0.0174532
#define FULL_CIRCLE 360

typedef void (*LCD_ScreenFunction_t)(LCD_COMMAND_T*);

static const int8_t octahedron_vertex[6][3] = { { 0, 30, 0 }, { -20, 0, 0 },
    { 0, 0, 20 }, { 20, 0, 0 }, { 0, 0, -20 }, { 0, -30, 0 } };

static uint16_t wireoctahedron[6][2];

static const uint8_t originx = 64;
static const uint8_t originy = 32;

uint32_t stime, fps, frames;
char string_fps[3];
int16_t angle;

/* Bouncing ball data */
uint8_t u8x = 10, u8y = 10;
int8_t s8speedx = 2, s8speedy = 2;

static LCD_COMMAND_T tLcdCommand = { 0 };

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

static void rotateOctahedron(int16_t pitch, int16_t roll, int16_t yaw)
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

static void DrawCircleSegment(uint8_t x0, uint8_t y0, const uint8_t radius, uint16_t startAngle, uint16_t endAngle)
{
  // Adjust angles with the offset and ensure they remain within 0-360°
  startAngle %= (FULL_CIRCLE + startAngle);
  endAngle %= (FULL_CIRCLE + startAngle);

  // Draw points in the specified angle range
  for(uint16_t angle = startAngle; angle <= endAngle; ++angle)
  {
    // Convert angle to radians
    float rad = angle * PI_180;

    // Calculate pixel positions using cos() and sin()
    int16_t x = x0 + (int16_t) (radius * cosf(rad)); // X-Position
    int16_t y = y0 - (int16_t) (radius * sinf(rad)); // Y-Position

    ssd1306_DrawPixel(x, y, White);
  }
}

static void DrawLoadingAnimation(uint8_t x0, uint8_t y0, uint8_t radius, uint32_t delay_ms)
{
  uint16_t progressAngle = 0;          /* Start at 0 degrees */
  const uint16_t startAngle = 89;
  const uint16_t fullCircle = FULL_CIRCLE + startAngle;    /* Full circle in degrees */
  const uint8_t stepAngle = 15;      /* Step size per iteration */

  while(progressAngle <= fullCircle)
  {
    /* Draw the new progress segment with the given start- and process angle */
    DrawCircleSegment(x0, y0, radius, startAngle, progressAngle);

    ssd1306_UpdateScreen();

    /* Increment the progress angle */
    progressAngle += stepAngle;

    /* Delay for smooth animation */
    vTaskDelay(delay_ms);
  }
}

static void ShowIdleScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);
  ssd1306_WriteString("Idle screen\n", Font_6x8, White);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(ptLcdPaket->pcMessage, Font_6x8, White);

  ssd1306_UpdateScreen();
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void ShowBootScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);

  ssd1306_WriteString("Boot screen\n", Font_6x8, White);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(ptLcdPaket->pcMessage, Font_6x8, White);

  ssd1306_UpdateScreen();
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void ShowVertexScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);

  if(angle > FULL_CIRCLE)
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
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void ShowBouncingBallScreen(LCD_COMMAND_T *ptLcdPaket)
{
  ssd1306_Fill(Black);
  ssd1306_GFX_FillCircle(u8x, u8y, 3, White);
  u8x += s8speedx;
  u8y += s8speedy;

  // Bounce from edges
  if(u8x <= 5 || u8x >= SSD1306_WIDTH - 5)
    s8speedx = -s8speedx;
  if(u8y <= 5 || u8y >= SSD1306_HEIGHT - 5)
    s8speedy = -s8speedy;


  ssd1306_UpdateScreen();
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void ShowConfigScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);
  ssd1306_WriteString("Checking config.\n", Font_6x8, White);
  DrawLoadingAnimation(originx, originy, 14, 8);
  ssd1306_UpdateScreen();
  vTaskDelay(pdMS_TO_TICKS(10));
}

static void ShowIoExchangeScreen(LCD_COMMAND_T* ptLcdPaket)
{
  for (uint8_t i = 0; i < 13; ++i)
  {
    ssd1306_Fill(White);
    ssd1306_GFX_DrawBitMap(0, 0, cat_bitmapallArray[i], SSD1306_WIDTH, SSD1306_HEIGHT, Black);
    ssd1306_UpdateScreen();
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

static void ShowErrorScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);
  ssd1306_WriteString("Error screen\n", Font_6x8, White);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(ptLcdPaket->pcMessage, Font_6x8, White);
  ssd1306_UpdateScreen();
  vTaskDelay(pdMS_TO_TICKS(10));
}

bool LCD_SendCommand(const LCD_COMMAND_T *ptCommand)
{
  if(tLcdCommand.eScreen == ptCommand->eScreen  ||  /* guard */
    (ptCommand->eScreen >= LCD_SCREEN_COUNT)    ||
    (NULL == ptCommand))
  {
    return false;
  }

  tLcdCommand = *ptCommand;
  return true;
}

void LCD_Worker(void *pvParameters)
{
  LCD_ScreenFunction_t screenFunctions[] = {
      [LCD_IDLE_SCREEN]       = ShowIdleScreen,
      [LCD_BOOT_SCREEN]       = ShowBootScreen,
      [LCD_CONFIG_SCREEN]     = ShowConfigScreen,
      [LCD_VERTEX_SCREEN]     = ShowBouncingBallScreen, //,ShowBouncingBallScreen
      [LCD_ERROR_SCREEN]      = ShowErrorScreen,
      [LCD_IOXCHANGE_SCREEN]  = ShowIoExchangeScreen
  };

  tLcdCommand.eScreen = LCD_IDLE_SCREEN;

  ssd1306_Init();
  while(1)
  {
    if(tLcdCommand.eScreen < LCD_SCREEN_COUNT)
    {
      screenFunctions[tLcdCommand.eScreen](&tLcdCommand);
    }
    else
    {
      ssd1306_WriteString("Unknown CMD received.", Font_6x8, White);
      ShowIdleScreen(&tLcdCommand);
      ssd1306_UpdateScreen();
      vTaskDelay(pdMS_TO_TICKS(10));
    }
    //ssd1306_SetCursor(0, 0);
  }
}
