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

/*************************************************************************************/

typedef struct PROTOCOL_IDENTIFICATION_Ttag
{
  const char* szFirmwareName;
  const uint16_t usFirmwareMajor;
  const uint16_t usFirmwareMinor;
} PROTOCOL_IDENTIFICATION_T;

/*************************************************************************************/
/*************************************************************************************/

typedef __HIL_PACKED_PRE struct PROCESS_DATA_INPUT_Ttag /* Consumed data (PLC Output) */
{
  uint16_t usRotationAngle_Input;   /* angle for the rotation of the lcd object */
  uint8_t  bReserved_Input;         /* later for the neopixel command */
  uint8_t  bReserved2_Input;        /* reserved for later application */

  #define APP_CYCLE_COUNTER_COUNT_UP    0
  #define APP_CYCLE_COUNTER_COUNT_DOWN  1
  uint8_t  bCyclicCounter_Direction;  /* handling of global counter variable (usCyclic_Counter): up/down counter
                                         0: count up,
                                         1: count down */

  uint8_t  bCyclicCounter_Speed; /* handling of global counter variable (usCyclic_Counter): fast/slow increment/decrement
                                      0: the counter is not incremented/decremented at all (the counter will show its default value 0xFFFF)
                                    > 0: number of cycles it takes to increment/decrement the counter by one. */
} __HIL_PACKED_POST PROCESS_DATA_INPUT_T;

typedef __HIL_PACKED_PRE struct PROCESS_DATA_OUTPUT_Ttag /* Produced data (PLC Input) */
{
  uint16_t usCyclicCounter;   /* global counter variable */
  uint16_t usSensor_1_Output;
  uint8_t  bSensor_1_State;
  uint16_t usSensor_2_Output;
  uint8_t  bSensor_2_State;
  uint8_t  bActuator_1_State;
  uint8_t  bActuator_2_State;
} __HIL_PACKED_POST PROCESS_DATA_OUTPUT_T;

typedef struct PNS_RESSOURCES_Ttag
{
  CIFXHANDLE            hCifXChannel;
  CHANNEL_INFORMATION   *ptCifXChannelInfo;

  PKT_INTERFACE_H       hPktIfRsc;
  CIFX_PACKET           tPacket;

  bool                  fDeviceIsRunning; /* device was configured */

  PROCESS_DATA_INPUT_T  tInputData;  /** Consumed process data. Data that is received from the PLC. */
  PROCESS_DATA_OUTPUT_T tOutputData; /** Produced process data. Data that is sent to the PLC.       */

} PNS_RESSOURCES_T;

uint32_t  PNS_ConfigureStack(PNS_RESSOURCES_T* ptRsc);
uint32_t  PNS_SetOemData(PNS_RESSOURCES_T* ptRsc);
uint32_t  PNS_ActivateDdp(PNS_RESSOURCES_T* ptRsc);
uint32_t  PNS_SetMacAddress(PNS_RESSOURCES_T* ptRsc);
bool      PNS_PacketHandler(CIFX_PACKET* ptPacket, void* pvUserData);

#endif /* INC_PNS_FUNCTIONS_H_ */
