/*
 * iodevice.h
 *
 *  Created on: May 7, 2025
 *      Author: link
 */

#ifndef STACKS_PNS_INC_IODEVICE_H_
#define STACKS_PNS_INC_IODEVICE_H_

#include "cifXUser.h"
#include "PNSIF_API.h"

#define PROCESS_DATA_TIMEOUT    10  /* milliseconds */
#define NETX_TXRX_PKT_TIMEOUT   500 /* milliseconds */

typedef struct PNSDeviceRsc_tag
{
  CIFXHANDLE          hChannel;
  CHANNEL_INFORMATION *ptNetxChannelInfo;
  CIFX_PACKET         tPacket;

  /* STX -> PLC : Input data                                                */
  /* TODO: for future use */
  uint32_t            ulReserved0;        /* MPU6050 pitch 4 bytes          */
  uint32_t            ulReserved1;        /* MPU6050 roll 4 bytes           */
  uint32_t            ulReserved2;        /* STX Buttons 4 bytes            */
  uint32_t            ulReserved3;        /* STX Temperature sensor 4 bytes */

  /* PLC -> STX : Output data     */
  uint16_t            usLcdCommandOut;    /* LCD command 2 bytes            */
  uint16_t            usLcdRotAngleOut;   /* LCD rotation angle 2 bytes     */

} PNSDeviceRsc_t; /* Profinet resource struct */

#endif /* STACKS_PNS_INC_IODEVICE_H_ */
