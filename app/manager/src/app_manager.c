/*
 * app_worker.c
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#include "app_manager.h"
#include "led_worker.h"
#include "lcd_worker.h"
#include "neopixel_worker.h"

static APP_MANANGER_RSC_T s_tAppRsc;

#define LED_QUEUE_LEN     1
#define LCD_QUEUE_LEN     1
#define NEOPXL_QUEUE_LEN  1

void AppManager_Init()
{
  OS_Memset(&s_tAppRsc, 0, sizeof(APP_MANANGER_RSC_T));

  s_tAppRsc.tAppQueues.ledQueue = xQueueCreate(LED_QUEUE_LEN, sizeof(eLedCommand));
  s_tAppRsc.tAppQueues.lcdQueue = xQueueCreate(LCD_QUEUE_LEN, sizeof(LCD_COMMAND_T));
  s_tAppRsc.tAppQueues.neopixelQueue = xQueueCreate(NEOPXL_QUEUE_LEN, sizeof(NEOPXL_DATA_ITEM_T));

  FreeRTOS_THREAD_T taskConfigs[] = {
    { (pdTASK_CODE)NetxWorker, "netx90 Task", configMINIMAL_STACK_SIZE * 24, (void*)s_tAppRsc.ptNetxRsc, (tskIDLE_PRIORITY) + 2, NULL },
    { (pdTASK_CODE)LCD_Worker, "LCD Task", configMINIMAL_STACK_SIZE * 3, (void*)s_tAppRsc.tAppQueues.lcdQueue, (tskIDLE_PRIORITY) + 0, NULL },
    { (pdTASK_CODE)LED_Worker, "Conf Led Task", configMINIMAL_STACK_SIZE, (void*)s_tAppRsc.tAppQueues.ledQueue, (tskIDLE_PRIORITY) + 3, NULL },
    { (pdTASK_CODE)Neopxl_Worker, "Neopixel Task", configMINIMAL_STACK_SIZE * 2, (void*)s_tAppRsc.tAppQueues.neopixelQueue, (tskIDLE_PRIORITY) + 1, NULL },
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
  (void)LED_SendCommand(&ptNetxRsc->tLedCmd);
  (void)LCD_SendCommand(&ptNetxRsc->tLcdCommand);
}

void AppManager_UpdateNeopixelDataFromPLC(NETX_APP_RSC_T* ptNetxRsc)
{
  NEOPXL_DATA_ITEM_T *ptNeopxlData = (NEOPXL_DATA_ITEM_T*)ptNetxRsc->atCommChannels[REALTIME_ETH_CHANNEL]->abActorData;
  (void)Neopxl_UpdateData(ptNeopxlData);
}

void AppManager_CallRedFlashingMode(NETX_APP_RSC_T* ptNetxRsc)
{
  NEOPXL_DATA_ITEM_T tNeopxlData;
  tNeopxlData.eMode = NEOPXL_FLASHING_2_MODE;
  tNeopxlData.tColor = (NEOPXL_RGB_T){ 0x40, 0, 0 };
  Neopxl_UpdateData(&tNeopxlData);
}
