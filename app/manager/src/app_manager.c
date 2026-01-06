/*
 * app_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */
#include "app_manager.h"
#include "task_messaging.h"
#include "led_worker.h"
#include "lcd_worker.h"
#include "neopixel_worker.h"

static APP_MANANGER_RSC_T s_tAppRsc;

void AppManager_Init()
{
  memset(&s_tAppRsc, 0, sizeof(APP_MANANGER_RSC_T));
  s_tAppRsc.fInitialized = false;

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
    { (pdTASK_CODE)NetxWorker, "netx90 Task", configMINIMAL_STACK_SIZE * 24, (void*)s_tAppRsc.ptNetxRsc, (tskIDLE_PRIORITY) + 2, NULL },
    { (pdTASK_CODE)LCD_Worker, "LCD Task", configMINIMAL_STACK_SIZE * 3, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LCD_WORKER].xtQueueHandle, (tskIDLE_PRIORITY) + 0, NULL },
    { (pdTASK_CODE)LED_Worker, "Conf Led Task", configMINIMAL_STACK_SIZE, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LED_WORKER].xtQueueHandle, (tskIDLE_PRIORITY) + 3, NULL },
    { (pdTASK_CODE)Neopxl_Worker, "Neopixel Task", configMINIMAL_STACK_SIZE * 2, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER].xtQueueHandle, (tskIDLE_PRIORITY) + 1, NULL },
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

void AppManager_UpdatePeripherals(NETX_APP_RSC_T* ptNetxRsc)
{
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LED_WORKER].pfnCmdCallback((void*)&ptNetxRsc->tLedCmd);
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LCD_WORKER].pfnCmdCallback((void*)&ptNetxRsc->tLcdCommand);
}

void AppManager_UpdateNeopixelDataFromPLC(NETX_APP_RSC_T* ptNetxRsc)
{
  NEOPXL_DATA_ITEM_T *ptNeopxlData = (NEOPXL_DATA_ITEM_T*)ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->abActorData;
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER].pfnCmdCallback((void*)ptNeopxlData);
}

void AppManager_CallRedFlashingMode(NETX_APP_RSC_T* ptNetxRsc)
{
  NEOPXL_DATA_ITEM_T tNeopxlData;
  tNeopxlData.eMode = NEOPXL_FLASHING_2_MODE;
  tNeopxlData.tColor = (NEOPXL_RGB_T){ 0x40, 0, 0 };
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER].pfnCmdCallback((void*)&tNeopxlData);
}
