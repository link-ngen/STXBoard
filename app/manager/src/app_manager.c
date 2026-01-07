/*
 * app_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */
#include <stdio.h>
#include "app_manager.h"
#include "task_messaging.h"
#include "led_worker.h"
#include "lcd_worker.h"
#include "neopixel_worker.h"

static APP_MANANGER_RSC_T s_tAppRsc;

static bool prvSendMessage(const void *pvUserData, TickType_t xTicksToWait)
{
  APP_COMMAND_E *peAppMsg = (APP_COMMAND_E*)pvUserData;
  if (pvUserData == NULL || *peAppMsg >= APP_CMD_COUNT) return false;

  // Avoid sending duplicate commands
//  if (s_tAppRsc.eCurrentCmd == *peAppMsg) return true;

  s_tAppRsc.eCurrentCmd = *peAppMsg;
  return xQueueSend(s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_APP_MANAGER].xtQueueHandle, pvUserData, xTicksToWait);
}

static void prvProcessMessage(const APP_COMMAND_E *eMessage, APP_MANANGER_RSC_T *ptAppMan)
{
  switch (*eMessage)
  {
  case APP_CMD_NETX_INIT_ERR:
    snprintf(ptAppMan->tLcdCommand.pcMessage, sizeof(ptAppMan->tLcdCommand.pcMessage), "DrvOpen err \n");
    ptAppMan->tLedCmd = LCD_ERROR_SCREEN;
    ptAppMan->tLcdCommand.eScreen = LCD_ERROR_SCREEN;
    TaskMsg_SendTo(QUEUE_ID_LED_WORKER, (void*)&ptAppMan->tLedCmd, 0);
    TaskMsg_SendTo(QUEUE_ID_LCD_WORKER, (void*)&ptAppMan->tLcdCommand, 0);
    break;

  case APP_CMD_NETX_CONFIG_ERR:
    snprintf(ptAppMan->tLcdCommand.pcMessage, sizeof(ptAppMan->tLcdCommand.pcMessage), "NetX config error\n");
    ptAppMan->tLedCmd = LCD_ERROR_SCREEN;
    ptAppMan->tLcdCommand.eScreen = LCD_ERROR_SCREEN;
    TaskMsg_SendTo(QUEUE_ID_LED_WORKER, (void*)&ptAppMan->tLedCmd, 0);
    TaskMsg_SendTo(QUEUE_ID_LCD_WORKER, (void*)&ptAppMan->tLcdCommand, 0);
    break;

  case APP_CMD_NETX_DPM_ERR:
    snprintf(s_tAppRsc.tLcdCommand.pcMessage, sizeof(ptAppMan->tLcdCommand.pcMessage), "NetX DPM error\n");
    ptAppMan->tLedCmd = LCD_ERROR_SCREEN;
    ptAppMan->tLcdCommand.eScreen = LCD_ERROR_SCREEN;
    TaskMsg_SendTo(QUEUE_ID_LED_WORKER, (void*)&ptAppMan->tLedCmd, 0);
    TaskMsg_SendTo(QUEUE_ID_LCD_WORKER, (void*)&ptAppMan->tLcdCommand, 0);
    break;

  case APP_CMD_NETX_PLC_DISCONNECTED:
  {
    ptAppMan->tLedCmd = LED_CMD_CONFIGURED;
    ptAppMan->tLcdCommand.eScreen = LCD_VERTEX_SCREEN;

    NEOPXL_DATA_ITEM_T tNeopxlData;
    tNeopxlData.eMode = NEOPXL_FLASHING_2_MODE;
    tNeopxlData.tColor = (NEOPXL_RGB_T ) { 0x40, 0, 0 };
    TaskMsg_SendTo(QUEUE_ID_LED_WORKER, (void*)&ptAppMan->tLedCmd, 0);
    TaskMsg_SendTo(QUEUE_ID_LCD_WORKER, (void*)&ptAppMan->tLcdCommand, 0);
    TaskMsg_SendTo(QUEUE_ID_NEOPIXEL_WORKER, (void*)&tNeopxlData, 0);
  }
    break;

  case APP_CMD_NETX_INIT_OK:
    snprintf(ptAppMan->tLcdCommand.pcMessage, sizeof(ptAppMan->tLcdCommand.pcMessage), "DrvOpen ok\n");
    ptAppMan->tLedCmd = LED_CMD_CONFIGURING;
    ptAppMan->tLcdCommand.eScreen = LCD_CONFIG_SCREEN;
    TaskMsg_SendTo(QUEUE_ID_LED_WORKER, (void*)&ptAppMan->tLedCmd, 0);
    TaskMsg_SendTo(QUEUE_ID_LCD_WORKER, (void*)&ptAppMan->tLcdCommand, 0);
    break;

  case APP_CMD_NETX_CONFIG_OK:
    ptAppMan->tLedCmd = LED_CMD_CONFIGURED;
    ptAppMan->tLcdCommand.eScreen = LCD_VERTEX_SCREEN;
    TaskMsg_SendTo(QUEUE_ID_LED_WORKER, (void*)&ptAppMan->tLedCmd, 0);
    TaskMsg_SendTo(QUEUE_ID_LCD_WORKER, (void*)&ptAppMan->tLcdCommand, 0);
    break;

  case APP_CMD_NETX_PLC_CONNECTED:
    ptAppMan->tLedCmd = LED_CMD_RUN_ON;
    ptAppMan->tLcdCommand.eScreen = LCD_IOXCHANGE_SCREEN;
    TaskMsg_SendTo(QUEUE_ID_LED_WORKER, (void*)&ptAppMan->tLedCmd, 0);
    TaskMsg_SendTo(QUEUE_ID_LCD_WORKER, (void*)&ptAppMan->tLcdCommand, 0);
    break;

  case APP_CMD_NETX_UPDATE_IODATA:
    TaskMsg_SendTo(QUEUE_ID_NEOPIXEL_WORKER, ptAppMan->ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->abActorData, 0);
    break;

  case APP_CMD_NETX_GENERAL_ERR:
  default:
    break;
  }
}

static void prvAppManagerTask(void* pvParameters)
{
  APP_COMMAND_E eAppCmd;
  APP_MANANGER_RSC_T *ptAppMan = (APP_MANANGER_RSC_T*)pvParameters;
  QueueHandle_t hQueueHandle = ptAppMan->ptTaskQueueConfig[QUEUE_ID_APP_MANAGER].xtQueueHandle;

  while (1)
  {
    if (xQueueReceive(hQueueHandle, &eAppCmd, portMAX_DELAY) == pdPASS)
    {
      prvProcessMessage(&eAppCmd, ptAppMan);
    }
  }
}

void AppManager_Init()
{
  memset(&s_tAppRsc, 0, sizeof(APP_MANANGER_RSC_T));
  s_tAppRsc.fInitialized = false;

  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_APP_MANAGER] = (QUEUE_CONFIG_T ) {
                                                       .xtQueueHandle = NULL,
                                                       .ulQueueLength = MSG_APP_MAN_QUEUE_LEN,
                                                       .ulItemSize = sizeof(APP_COMMAND_E),
                                                       .pfnCmdCallback = prvSendMessage, };

  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LED_WORKER] = (QUEUE_CONFIG_T ) {
                                                     .xtQueueHandle = NULL,
                                                     .ulQueueLength = MSG_LED_QUEUE_LEN,
                                                     .ulItemSize = sizeof(LED_COMMAND_E),
                                                     .pfnCmdCallback = LED_SendCommand, };

  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LCD_WORKER] = (QUEUE_CONFIG_T ) {
                                                     .xtQueueHandle = NULL,
                                                     .ulQueueLength = MSG_LCD_QUEUE_LEN,
                                                     .ulItemSize = sizeof(LCD_COMMAND_T),
                                                     .pfnCmdCallback = LCD_SendCommand, };

  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER] = (QUEUE_CONFIG_T ) {
                                                     .xtQueueHandle = NULL,
                                                     .ulQueueLength = MSG_NEOPXL_QUEUE_LEN,
                                                     .ulItemSize = sizeof(NEOPXL_DATA_ITEM_T),
                                                     .pfnCmdCallback = Neopxl_UpdateData, };
  if (!TaskMsg_Init(s_tAppRsc.ptTaskQueueConfig))
  {
    return;
  }

  FreeRTOS_THREAD_T taskConfigs[] = {
    { (pdTASK_CODE)prvAppManagerTask, "AppMan Task", configMINIMAL_STACK_SIZE * 4, (void*)&s_tAppRsc, (tskIDLE_PRIORITY) + 4, NULL },
    { (pdTASK_CODE)LED_Worker, "Conf Led Task", configMINIMAL_STACK_SIZE, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LED_WORKER].xtQueueHandle, (tskIDLE_PRIORITY) + 3, NULL },
    { (pdTASK_CODE)NetxWorker, "netx90 Task", configMINIMAL_STACK_SIZE * 24, (void*)s_tAppRsc.ptNetxRsc, (tskIDLE_PRIORITY) + 2, NULL },
    { (pdTASK_CODE)Neopxl_Worker, "Neopixel Task", configMINIMAL_STACK_SIZE * 2, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER].xtQueueHandle, (tskIDLE_PRIORITY) + 1, NULL },
    { (pdTASK_CODE)LCD_Worker, "LCD Task", configMINIMAL_STACK_SIZE * 3, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LCD_WORKER].xtQueueHandle, (tskIDLE_PRIORITY), NULL },
  };

  BaseType_t xReturned = pdPASS;
  for (uint32_t i = 0; i < sizeof(taskConfigs) / sizeof(taskConfigs[0]); ++i)
  {
    xReturned = xTaskCreate( taskConfigs[i].pfnThread,
                             taskConfigs[i].pcName,
                             taskConfigs[i].usStackDepth,
                             taskConfigs[i].pvArg,
                             taskConfigs[i].uxPriority,
                             &taskConfigs[i].hThread);
    configASSERT(pdPASS == xReturned);
  }
  s_tAppRsc.fInitialized = true;
}

void AppManager_Run()
{
  if (s_tAppRsc.fInitialized)
    vTaskStartScheduler();
  else   /* catch errors */
    Error_Handler();
}

void AppManager_SendCommand(APP_COMMAND_E eMessage)
{
  TaskMsg_SendTo(QUEUE_ID_APP_MANAGER, (void*)&eMessage, 10);
}
