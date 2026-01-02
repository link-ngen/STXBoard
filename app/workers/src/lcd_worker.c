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

typedef struct LCD_STATE_Ttag
{
  union
  {
    /* Boucing ball data*/
    struct
    {
      uint8_t u8x, u8y;
      int8_t s8speedx, s8speedy;
    } tBouncingBall;

    /* Loading Animation */
    struct
    {
      uint16_t uProgressAngle;
      bool bActive;
    } tLoadingAnimation;

    /* IO Exchange (cat animation) */
    struct
    {
      uint8_t uFrameIndex;
      uint32_t uLastFrameTime;
    } tAnimation;
  } uScreenData;

  uint32_t ulLastUpdate;
  uint32_t ulUpdateInterval;
  LCD_SCREEN_E eCurrentScreen;
  LCD_COMMAND_T *tCurrentCommand;
} LCD_STATE_T;

static LCD_STATE_T *s_ptLcdState;
static QueueHandle_t s_pxLcdQueue = NULL;

#if 0
uint32_t stime, fps, frames;
char string_fps[3];
int16_t angle;

static const int8_t octahedron_vertex[6][3] = { { 0, 30, 0 }, { -20, 0, 0 },
    { 0, 0, 20 }, { 20, 0, 0 }, { 0, 0, -20 }, { 0, -30, 0 } };

static uint16_t wireoctahedron[6][2];

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
#endif


static void LCD_InitializeScreen(LCD_SCREEN_E eScreen)
{
  memset(&s_ptLcdState->uScreenData, 0, sizeof(s_ptLcdState->uScreenData));
  s_ptLcdState->eCurrentScreen = eScreen;

  switch (eScreen)
  {
  case LCD_VERTEX_SCREEN:
    s_ptLcdState->uScreenData.tBouncingBall.u8x = 10;
    s_ptLcdState->uScreenData.tBouncingBall.u8y = 10;
    s_ptLcdState->uScreenData.tBouncingBall.s8speedx = 2;
    s_ptLcdState->uScreenData.tBouncingBall.s8speedy = 2;
    s_ptLcdState->ulUpdateInterval = pdMS_TO_TICKS(8);
    break;

  case LCD_IOXCHANGE_SCREEN:
    s_ptLcdState->uScreenData.tAnimation.uFrameIndex = 0;
    s_ptLcdState->uScreenData.tAnimation.uLastFrameTime = 0;
    s_ptLcdState->ulUpdateInterval = pdMS_TO_TICKS(50);
    break;

  case LCD_CONFIG_SCREEN:
    /* Loading Animation */
    s_ptLcdState->uScreenData.tLoadingAnimation.uProgressAngle = 0;
    s_ptLcdState->uScreenData.tLoadingAnimation.bActive = true;
    s_ptLcdState->ulUpdateInterval = pdMS_TO_TICKS(10); /* Loading */
    break;

  case LCD_IDLE_SCREEN:
  case LCD_BOOT_SCREEN:
  case LCD_ERROR_SCREEN:
  default:
    s_ptLcdState->ulUpdateInterval = pdMS_TO_TICKS(1000); /* slow Update */
    break;
  }
  s_ptLcdState->ulLastUpdate = xTaskGetTickCount();
}

static bool LCD_IsTimeForUpdate(void)
{
  uint32_t ulCurrentTime = xTaskGetTickCount();
  return ((ulCurrentTime - s_ptLcdState->ulLastUpdate) >= s_ptLcdState->ulUpdateInterval);
}

static void DrawCircleSegment(uint8_t x0, uint8_t y0, const uint8_t radius, uint16_t startAngle, uint16_t endAngle)
{
  for(uint16_t angle = startAngle; angle <= endAngle; ++angle)
  {
    float rad = angle * PI_180;
    int16_t x = x0 + (int16_t) (radius * cosf(rad));
    int16_t y = y0 - (int16_t) (radius * sinf(rad));
    ssd1306_DrawPixel(x, y, White);
  }
}

static void ShowIdleScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Idle\n", Font_6x8, White);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(ptLcdPaket->pcMessage, Font_6x8, White);
  ssd1306_UpdateScreen();
}

static void ShowBootScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Booting...\n", Font_6x8, White);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(ptLcdPaket->pcMessage, Font_6x8, White);
  ssd1306_UpdateScreen();
}

static void ShowBouncingBallScreen(LCD_COMMAND_T *ptLcdPaket)
{
  /* Bouncing Ball */
  ssd1306_Fill(Black);
  ssd1306_GFX_FillCircle(s_ptLcdState->uScreenData.tBouncingBall.u8x, s_ptLcdState->uScreenData.tBouncingBall.u8y, 3, White);

  /* movement */
  s_ptLcdState->uScreenData.tBouncingBall.u8x += s_ptLcdState->uScreenData.tBouncingBall.s8speedx;
  s_ptLcdState->uScreenData.tBouncingBall.u8y += s_ptLcdState->uScreenData.tBouncingBall.s8speedy;

  /* border collision */
  if(s_ptLcdState->uScreenData.tBouncingBall.u8x <= 5 || s_ptLcdState->uScreenData.tBouncingBall.u8x >= SSD1306_WIDTH - 5)
  {
    s_ptLcdState->uScreenData.tBouncingBall.s8speedx = -s_ptLcdState->uScreenData.tBouncingBall.s8speedx;
  }

  if(s_ptLcdState->uScreenData.tBouncingBall.u8y <= 5 || s_ptLcdState->uScreenData.tBouncingBall.u8y >= SSD1306_HEIGHT - 5)
  {
    s_ptLcdState->uScreenData.tBouncingBall.s8speedy = -s_ptLcdState->uScreenData.tBouncingBall.s8speedy;
  }
  ssd1306_UpdateScreen();
}

static void ShowConfigScreen(LCD_COMMAND_T* ptLcdPaket)
{
  static const uint8_t CENTER_X     = 64;
  static const uint8_t CENTER_Y     = 32;
  static const uint8_t RADIUS       = 14;
  static const uint16_t START_ANGLE = 89;
  static const uint8_t STEP_ANGLE   = 15;

  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Checking config.", Font_6x8, White);

  /* Loading Animation */
  if(s_ptLcdState->uScreenData.tLoadingAnimation.bActive)
  {
    DrawCircleSegment(CENTER_X, CENTER_Y, RADIUS, START_ANGLE, s_ptLcdState->uScreenData.tLoadingAnimation.uProgressAngle);

    s_ptLcdState->uScreenData.tLoadingAnimation.uProgressAngle += STEP_ANGLE;

    if(s_ptLcdState->uScreenData.tLoadingAnimation.uProgressAngle >= (FULL_CIRCLE + START_ANGLE))
    {
      s_ptLcdState->uScreenData.tLoadingAnimation.uProgressAngle = 0;
      s_ptLcdState->uScreenData.tLoadingAnimation.bActive = false; /* Animation finished */
    }
  }
  ssd1306_UpdateScreen();
}

static void ShowIoExchangeScreen(LCD_COMMAND_T* ptLcdPaket)
{
  if(s_ptLcdState->uScreenData.tAnimation.uFrameIndex < cat_bitmapallArray_LEN)
  {
    ssd1306_Fill(White);
    ssd1306_GFX_DrawBitMap(0, 0, cat_bitmapallArray[s_ptLcdState->uScreenData.tAnimation.uFrameIndex],
    SSD1306_WIDTH, SSD1306_HEIGHT, Black);

    ++s_ptLcdState->uScreenData.tAnimation.uFrameIndex;
    if(s_ptLcdState->uScreenData.tAnimation.uFrameIndex >= cat_bitmapallArray_LEN)
    {
      s_ptLcdState->uScreenData.tAnimation.uFrameIndex = 0;
    }
    ssd1306_UpdateScreen();
  }
}

static void ShowErrorScreen(LCD_COMMAND_T* ptLcdPaket)
{
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("ERROR", Font_6x8, White);
  ssd1306_SetCursor(0, 10);
  ssd1306_WriteString(ptLcdPaket->pcMessage, Font_6x8, White);
  ssd1306_UpdateScreen();
}

bool LCD_SendCommand(const LCD_COMMAND_T *ptCommand)
{
  if(s_pxLcdQueue == NULL ||
    ptCommand == NULL ||
    ptCommand->eScreen >= LCD_SCREEN_COUNT)
  {
    return false;
  }

  LCD_COMMAND_T tCopy = *ptCommand;
  return (xQueueOverwrite(s_pxLcdQueue, &tCopy) == pdPASS);
}

void LCD_Worker(void *pvParameters)
{
  s_pxLcdQueue = (QueueHandle_t)pvParameters;

  LCD_ScreenFunction_t screenFunctions[] = {
      [LCD_IDLE_SCREEN]       = ShowIdleScreen,
      [LCD_BOOT_SCREEN]       = ShowBootScreen,
      [LCD_CONFIG_SCREEN]     = ShowConfigScreen,
      [LCD_VERTEX_SCREEN]     = ShowBouncingBallScreen, //,ShowBouncingBallScreen
      [LCD_ERROR_SCREEN]      = ShowErrorScreen,
      [LCD_IOXCHANGE_SCREEN]  = ShowIoExchangeScreen
  };

  LCD_COMMAND_T tNewCommand;
  LCD_COMMAND_T tCurrentCommand = { .eScreen = LCD_IDLE_SCREEN, .pcMessage = "LCD Ready", .pvCustomData = NULL };

  s_ptLcdState->tCurrentCommand = &tCurrentCommand;

  ssd1306_Init();
  LCD_InitializeScreen(tCurrentCommand.eScreen);
  ShowIdleScreen(&tCurrentCommand);

  while(1)
  {
    if(xQueueReceive(s_pxLcdQueue, &tNewCommand, 0) == pdTRUE)
    {
      if(tNewCommand.eScreen < LCD_SCREEN_COUNT)
      {
        if(tCurrentCommand.eScreen != tNewCommand.eScreen)
        {
          LCD_InitializeScreen(tNewCommand.eScreen);
        }

        tCurrentCommand = tNewCommand;
        s_ptLcdState->tCurrentCommand = &tCurrentCommand;
        //s_tLcdState.ulLastUpdate = xTaskGetTickCount() - s_tLcdState.ulUpdateInterval;
      }
    }

    /* 2. check if it's time for update */
    if(LCD_IsTimeForUpdate())
    {
      if(s_ptLcdState->eCurrentScreen < LCD_SCREEN_COUNT)
      {
        screenFunctions[s_ptLcdState->eCurrentScreen](s_ptLcdState->tCurrentCommand);
        s_ptLcdState->ulLastUpdate = xTaskGetTickCount();
      }
    }

    /* 3. delay for cpu schedule */
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
