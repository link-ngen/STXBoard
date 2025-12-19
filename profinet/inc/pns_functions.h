/*
 * pns_functions.h
 *
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#ifndef INC_PNS_FUNCTIONS_H_
#define INC_PNS_FUNCTIONS_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "PacketCommunication.h"
#include "netx_worker.h"

#include "Hil_ApplicationCmd.h"
#include "Hil_SystemCmd.h"
#include "Hil_Results.h"
#include "Hil_Packet.h"
#include "Hil_DualPortMemory.h"
#include "Hil_ComponentID.h"
#include "Hil_DeviceProductionData.h"
#include "pns_firmware_version.h"

#include "PNSIF_API.h"

/*************************************************************************************/

#define TX_TIMEOUT       500 /* milliseconds */
#define TXRX_TIMEOUT     500 /* milliseconds */


#define PNS_PROCESS_DATA_INPUT_SIZE        6
#define PNS_PROCESS_DATA_OUTPUT_SIZE       10

/*************************************************************************************/

typedef struct PROTOCOL_IDENTIFICATION_Ttag
{
  const char* szFirmwareName;
  const uint16_t usFirmwareMajor;
  const uint16_t usFirmwareMinor;
} PROTOCOL_IDENTIFICATION_T;

uint32_t  PNS_ConfigureStack(NETX_PROTOCOL_RSC_T* ptProtocolRsc);
uint32_t  PNS_SetOemData(NETX_PROTOCOL_RSC_T* ptProtocolRsc);
uint32_t  PNS_ActivateDdp(NETX_PROTOCOL_RSC_T* ptProtocolRsc);
uint32_t  PNS_SetMacAddress(NETX_PROTOCOL_RSC_T* ptProtocolRsc);
bool      PNS_PacketHandler(CIFX_PACKET* ptPacket, void* pvUserData);

#endif /* INC_PNS_FUNCTIONS_H_ */
