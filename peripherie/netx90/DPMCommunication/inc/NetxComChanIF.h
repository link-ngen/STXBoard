/*
 * NetxChannel.h
 * Define channel function interface for the netx channels.
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#ifndef NETX90_DPMCOMMUNICATION_INC_NETXCOMCHANIF_H_
#define NETX90_DPMCOMMUNICATION_INC_NETXCOMCHANIF_H_

#include "Hil_Compiler.h"
#include "cifXUser.h"

/*! Handle to the protocol specific resources. */
typedef struct NETX_COMM_CHANNEL_HANDLER_RSC_Ttag* NETX_COMM_CHANNEL_HANDLER_RSC_H;

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
typedef int (*NetxComm_InitializeCallback)(NETX_COMM_CHANNEL_HANDLER_RSC_H *phCommChHdlRsc, CIFXHANDLE hCifXChannel, CHANNEL_INFORMATION *ptCifXChannelInfo);

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
typedef int (*NetxComm_SetupCallback)(NETX_COMM_CHANNEL_HANDLER_RSC_H hCommChHdlRsc);

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
typedef void (*NetxComm_CyclicTaskCallback)(NETX_COMM_CHANNEL_HANDLER_RSC_H hCommChHdlRsc);

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
typedef void (*NetxComm_DeInitializeCallback)(NETX_COMM_CHANNEL_HANDLER_RSC_H phCommChHdlRsc, CIFXHANDLE hCifXChannel);

/** Defines the Interface functions which the protocol entry points for the protocol specific functions */
typedef __HIL_PACKED_PRE struct NETX_COMM_CHANNEL_HANDLER_Ttag
{
  NetxComm_InitializeCallback    pfnInitialize;
  NetxComm_SetupCallback         pfnSetup;
  NetxComm_CyclicTaskCallback    pfnCyclicTask;
  NetxComm_DeInitializeCallback  pfnDeInitialize;
} __HIL_PACKED_POST NETX_COMM_CHANNEL_HANDLER_T;

#endif /* NETX90_DPMCOMMUNICATION_INC_NETXCOMCHANIF_H_ */
