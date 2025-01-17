/*
 * netx_worker.h
 *
 *  Created on: Dec 10, 2024
 *      Author: link
 */

#ifndef WORKERS_INC_NETX_WORKER_H_
#define WORKERS_INC_NETX_WORKER_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "cifXErrors.h"
#include "cifXToolkit.h"

#include "led_worker.h"
#include "lcd_worker.h"

/* netX firmware defines */
#define  COM_CHANNEL    0     /* Use the first Channel on the choosen board */
#define  CHANNEL_COUNT  COM_CHANNEL + 1

typedef struct NetxSysChannelRessource_tag
{
  CIFX_PACKET             tPacket;
  CIFXHANDLE              hSys;         /** Handle of netX DPM communication channel                */
  BOARD_INFORMATION       tBoardInfo;     /** DPM channel information. Read during application start. */
} NetxSysChannelRessource_t;

typedef struct NetxComChannelRessource_tag
{
  CIFX_PACKET             tPacket;
  CIFXHANDLE              hChannel;         /** Handle of netX DPM communication channel                */
  CHANNEL_INFORMATION     tChannelInfo;     /** DPM channel information. Read during application start. */
} NetxComChannelRessource_t;

struct NetxFsmRessource_tag;
typedef void (*NetxStateFunction_t)(struct NetxFsmRessource_tag *ptNetxRsc);

typedef struct NetxFsmRessource_tag
{
  CIFXHANDLE                  hDriver;
  NetxSysChannelRessource_t   tSysRsc;
  NetxComChannelRessource_t   tChannelRsc;
  bool                        fNetXDrvRunning;  /* netX driver is running */

  /**< netx state function */
  NetxStateFunction_t currentState;
  NetxStateFunction_t lastState;

  /* Queue handle to signal leds  */
  xQueueHandle xLedQueueHandle;
  LedTaskCommand_t tLedCmd;

  /* Queue handle to signal lcd  */
  xQueueHandle xLcdQueueHandle;
  LedTaskCommand_t tLcdCmd;

} NetxFsmRessource_t;

void NetxDemoWorker(void *pvParameters);

#endif /* WORKERS_INC_NETX_WORKER_H_ */
