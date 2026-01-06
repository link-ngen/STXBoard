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

  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LED_WORKER] = (QUEUE_CONFIG_T ) {
                                                     .handle = NULL,
                                                     .length = MSG_LED_QUEUE_LEN,
                                                     .itemSize = sizeof(LED_COMMAND_E),
                                                     .pfnHandler = LED_SendCommand,
  };

  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LCD_WORKER] = (QUEUE_CONFIG_T ) {
                                                     .handle = NULL,
                                                     .length = MSG_LCD_QUEUE_LEN,
                                                     .itemSize = sizeof(LCD_COMMAND_T),
                                                     .pfnHandler = LCD_SendCommand,
  };

  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER] = (QUEUE_CONFIG_T ) {
                                                     .handle = NULL,
                                                     .length = MSG_NEOPXL_QUEUE_LEN,
                                                     .itemSize = sizeof(NEOPXL_DATA_ITEM_T),
                                                     .pfnHandler = Neopxl_UpdateData,
  };

  if (!TaskMsg_Init(s_tAppRsc.ptTaskQueueConfig))
  {
    return;
  }

  FreeRTOS_THREAD_T taskConfigs[] = {
    { (pdTASK_CODE)NetxWorker, "netx90 Task", configMINIMAL_STACK_SIZE * 24, (void*)s_tAppRsc.ptNetxRsc, (tskIDLE_PRIORITY) + 2, NULL },
    { (pdTASK_CODE)LCD_Worker, "LCD Task", configMINIMAL_STACK_SIZE * 3, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LCD_WORKER].handle, (tskIDLE_PRIORITY) + 0, NULL },
    { (pdTASK_CODE)LED_Worker, "Conf Led Task", configMINIMAL_STACK_SIZE, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LED_WORKER].handle, (tskIDLE_PRIORITY) + 3, NULL },
    { (pdTASK_CODE)Neopxl_Worker, "Neopixel Task", configMINIMAL_STACK_SIZE * 2, (void*)s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER].handle, (tskIDLE_PRIORITY) + 1, NULL },
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
}

void AppManager_Run()
{
  vTaskStartScheduler();
  /* catch errors */
}

void AppManager_UpdatePeripherals(NETX_APP_RSC_T* ptNetxRsc)
{
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LED_WORKER].pfnHandler((void*)&ptNetxRsc->tLedCmd);
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_LCD_WORKER].pfnHandler((void*)&ptNetxRsc->tLcdCommand);
//  (void)LED_SendCommand(&ptNetxRsc->tLedCmd);
//  (void)LCD_SendCommand(&ptNetxRsc->tLcdCommand);
}

void AppManager_UpdateNeopixelDataFromPLC(NETX_APP_RSC_T* ptNetxRsc)
{
  NEOPXL_DATA_ITEM_T *ptNeopxlData = (NEOPXL_DATA_ITEM_T*)ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->abActorData;
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER].pfnHandler((void*)ptNeopxlData);
  //  (void)Neopxl_UpdateData(ptNeopxlData);
}

void AppManager_CallRedFlashingMode(NETX_APP_RSC_T* ptNetxRsc)
{
  NEOPXL_DATA_ITEM_T tNeopxlData;
  tNeopxlData.eMode = NEOPXL_FLASHING_2_MODE;
  tNeopxlData.tColor = (NEOPXL_RGB_T){ 0x40, 0, 0 };
  s_tAppRsc.ptTaskQueueConfig[QUEUE_ID_NEOPIXEL_WORKER].pfnHandler((void*)&tNeopxlData);
//  Neopxl_UpdateData(&tNeopxlData);
}
