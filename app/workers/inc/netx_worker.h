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

#include "NetxComChanIF.h"
#include "cifXErrors.h"
#include "cifXToolkit.h"

#include "led_worker.h"
#include "lcd_worker.h"

#define REALTIME_ETH_CHANNEL 0
/* netX firmware defines */
#define  USED_COMMUNICATION_CHANNELS  REALTIME_ETH_CHANNEL + 1

struct NetxRessource_tag;
typedef void (*NetxStateFunction_t)(struct NetxRessource_tag *ptNetxRsc);

typedef enum
{
  NETX_STATE_INIT,
  NETX_STATE_PREOP,
  NETX_STATE_OP,
//  NETX_STATE_FWUPDATE,
  NETX_STATE_ERROR,
  NETX_STATE_COUNT
} NetxStateId_e;

typedef struct
{
  NetxStateId_e id;
  NetxStateFunction_t pfnc;
} NetxStateDescriptor_t;

/** A communication channel struct.
 * This struct holds data related to one channel handler.
 */
typedef struct
{
  NETX_COMM_CHANNEL_HANDLER_T      tCommChannelHandler;  /** This is the communication channel handler for the cifX communication channel */
  NETX_COMM_CHANNEL_HANDLER_RSC_H  hCommChannelRsc;      /** Resource handle for the communication channel handler  */

  CIFXHANDLE                       hCifXChannel;        /** Handle of netX DPM communication channel                */
  CHANNEL_INFORMATION              tCifXChannelInfo;    /** DPM channel information. Read during application start. */
} NETX_COMM_CHANNEL_T;

typedef struct NetxRessource_tag
{
  CIFXHANDLE                      hDriver;
  NETX_COMM_CHANNEL_T             atCommChannels[USED_COMMUNICATION_CHANNELS];
  BOARD_INFORMATION               tCifXBoardInfo;   /** netX Board information. Read during application start.  */
  bool                            fNetXDrvRunning;  /* netX driver is running */
  uint8_t                         bInitErrCounter;

  /**< netx state function */
  const NetxStateDescriptor_t     *currentState;
  const NetxStateDescriptor_t     *previousState;

  eLedCommand                     tLedCmd;
  LcdCommand_t                    tLcdCommand;

  TaskHandle_t                    xMailboxTaskHandle;
} NetxRessource_t;

void NetxWorker(void *pvParameters);

#endif /* WORKERS_INC_NETX_WORKER_H_ */
