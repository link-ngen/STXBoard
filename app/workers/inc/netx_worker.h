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
#include "neopixel_worker.h"

#include "PacketCommunication.h"

#define REALTIME_ETH_CHANNEL 0
/* netX firmware defines */
#define  USED_COMMUNICATION_CHANNELS  REALTIME_ETH_CHANNEL + 1

#define PNS_PROCESS_DATA_INPUT_SIZE   6
#define PNS_PROCESS_DATA_OUTPUT_SIZE  10

struct NETX_APP_RSC_Ttag;
typedef void (*NetxStateFunction_t)(struct NETX_APP_RSC_Ttag *ptNetxRsc);

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

struct NETX_PROTOCOL_RSC_Ttag;
/* Initialization of communication channel
 *
 * The Initialization function will be called only once. Within this function internal resources can
 * be allocated and initialized. Those resources can be later references via the phCommChHdlRsc variable,
 * which will be passed in all other communication channel handler functions.
 *
 * This function can be used to initialize the netX communication LFW  e.g. Set DDP data (MAC,
 * serial number, OEM data, etc.), set remanent data or setting up security related configuration
 *
 * If the communication channel handler want to use e.g. xChannelRegisterNotification functions,
 * this is also a good pace to put this call here.
 *
 * \param phCommChHdlRsc     [out] Reference to the internal resources
 * \param hCifXChannel       [in]  Handle to the related cifXChannel
 * \param ptCifXChannelInfo  [in]  Channel information structure
 */
typedef int (*NetxComm_InitializeCallback)(struct NETX_PROTOCOL_RSC_Ttag *ptProtocolRsc, CIFXHANDLE hCifXChannel, CHANNEL_INFORMATION *ptCifXChannelInfo);

/* Setup communication channel
 *
 * The setup callback is called in order to send a configuration to the protocol stack or component.
 *
 * After the Bus on Signal is switched to "ON" the application needs to be able to generate IO-Data
 * (e.g. xChannelIOWrite) for the communication stack (otherwise some communication stacks will not
 * go "online".
 *
 * \param phCommChHdlRsc     [in] Reference to the internal resources
 */
typedef int (*NetxComm_SetupCallback)(struct NETX_PROTOCOL_RSC_Ttag *ptProtocolRsc);

/* Cyclic io task
 *
 * This callback will be called from a task once every millisecond. This can be used to e.g. to
 * exchange IO Data with the communication channel via xChannelIOWrite/xChannelIORead.
 * Note that if the function (all channel hander timer task function combined) needs more than 1ms
 * time, timer ticks may get lost!
 *
 * \note It's not allowed to block within this function!
 *
 * \param phCommChHdlRsc     [in] Reference to the internal resources
 */
typedef int (*NetxComm_CyclicTaskCallback)(struct NETX_PROTOCOL_RSC_Ttag *ptProtocolRsc);

/**
 * De-initialization of the communication channel.
 *
 * This callback function is called exactly once when a communication channel
 * is shut down. All internal resources allocated during initialization must be
 * released and de-initialized here.
 *
 * If the communication channel handler has registered notifications
 * (e.g., using xChannelRegisterNotification), this is also the right place
 * to unregister them.
 *
 * /param phCommChHdlRsc [in,out] Pointer to the internal channel resources to be released
 * /param hCifXChannel   [in]     Handle to the corresponding cifX channel.
 */
typedef void (*NetxComm_DeInitializeCallback)(struct NETX_PROTOCOL_RSC_Ttag *ptProtocolRsc, CIFXHANDLE hCifXChannel);

typedef void (*NetxComm_MailboxCallback)(struct NETX_PROTOCOL_RSC_Ttag *ptProtocolRsc);

typedef uint32_t (*NetxComm_ReadNetworkState)(struct NETX_PROTOCOL_RSC_Ttag *ptProtocolRsc);

/** Defines the Interface functions which the protocol entry points for the protocol specific functions */
typedef __HIL_PACKED_PRE struct NETX_COMM_CHANNEL_HANDLER_Ttag
{
  NetxComm_InitializeCallback    pfnInitialize;
  NetxComm_SetupCallback         pfnSetup;
  NetxComm_CyclicTaskCallback    pfnCyclicTask;
  NetxComm_MailboxCallback       pfnMailboxTask;
  NetxComm_ReadNetworkState      pfnReadNetworkState;
  NetxComm_DeInitializeCallback  pfnDeInitialize;
} __HIL_PACKED_POST NETX_PROTOCOL_DESC_T;

/** A communication channel struct.
 * This struct holds data related to one channel handler.
 */
typedef struct NETX_PROTOCOL_RSC_Ttag
{
  NETX_PROTOCOL_DESC_T             tProtocolDesc;       /** This is the communication channel function interface */

  CIFXHANDLE                       hCifXChannel;        /** Handle of netX DPM communication channel                */
  CHANNEL_INFORMATION              tCifXChannelInfo;    /** DPM channel information. Read during application start. */

  PKT_INTERFACE_H                  hPktIfRsc;
  CIFX_PACKET                      tPacket;

  bool                             fDeviceIsRunning;    /* device was configured */

//  PROCESS_DATA_INPUT_T             tInputData;          /** Consumed process data. Data that is received from the PLC. */
//  PROCESS_DATA_OUTPUT_T            tOutputData;         /** Produced process data. Data that is sent to the PLC.       */

  uint8_t abActorData[PNS_PROCESS_DATA_INPUT_SIZE];       /** Consumed process data. Data that is received from the PLC. */
  uint8_t abSensorData[PNS_PROCESS_DATA_OUTPUT_SIZE];     /** Produced process data. Data that is sent to the PLC.       */

} NETX_PROTOCOL_RSC_T; /* for example profinet data resource */

typedef struct NETX_APP_RSC_Ttag
{
  CIFXHANDLE                      hDriver;
  NETX_PROTOCOL_RSC_T             *atCommChannels[USED_COMMUNICATION_CHANNELS];
  BOARD_INFORMATION               tCifXBoardInfo;   /** netX Board information. Read during application start.  */
  bool                            fNetXDrvRunning;  /* netX driver is running */
  uint8_t                         bInitErrCounter;

  /**< netx state function */
  const NetxStateDescriptor_t     *currentState;
  const NetxStateDescriptor_t     *previousState;

  eLedCommand                     tLedCmd;
  LCD_COMMAND_T                   tLcdCommand;

  TaskHandle_t                    xMailboxTaskHandle;
} NETX_APP_RSC_T;

/* netx helper functions */
int32_t InitializeToolkit(NETX_APP_RSC_T *ptNetxRsc);
int32_t NetX_InitializeChannels(NETX_APP_RSC_T *ptNetxRsc, char *szBoardName);
int32_t NetX_ConfigureChannels(NETX_APP_RSC_T* ptNetxRsc);
void    NetX_CallCommMailboxRoutine(NETX_APP_RSC_T* ptNetxRsc);
void    NetX_AllChannels_Close(NETX_APP_RSC_T* ptNetxRsc);
uint32_t Netx_ReadNetworkState(NETX_APP_RSC_T *ptNetxRsc);
int32_t NetX_ProcessIOData(NETX_APP_RSC_T *ptNetxRsc);

/* netx worker */
void NetxWorker(void *pvParameters);

#endif /* WORKERS_INC_NETX_WORKER_H_ */
