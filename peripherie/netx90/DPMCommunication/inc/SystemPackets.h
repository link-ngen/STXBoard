/*
 * SystemPacket.h
 *
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#ifndef NETX90_DPMCOMMUNICATION_INC_SYSTEMPACKETS_H_
#define NETX90_DPMCOMMUNICATION_INC_SYSTEMPACKETS_H_

#include "app_config.h"
#include "cifXUser.h"

#include <stdint.h>
#include <stdbool.h>

/*****************************************************************************/
/*! FUNCTION PROTOTYPES                                                      */
/*****************************************************************************/

void    SysPkt_AssembleRegisterAppReq     (CIFX_PACKET* ptPkt);
void    SysPkt_AssembleUnRegisterAppReq   (CIFX_PACKET* ptPkt);
void    SysPkt_AssembleChannelInitReq     (CIFX_PACKET* ptPkt);
void    SysPkt_AssembleStartStopCommReq   (CIFX_PACKET* ptPkt, bool fStart);
void    SysPkt_AssembleSetMacAddressReq   (CIFX_PACKET* ptPkt, uint8_t* abMacAddr);
void    SysPkt_AssembleFirmwareIdentifyReq(CIFX_PACKET* ptPkt, uint32_t ulChannelId);

void    SysPkt_AssembleFirmwareResetReq   (CIFX_PACKET* ptPkt, uint8_t bResetMode, uint8_t bResetParam, uint8_t bDeleteRemanentData);
void    SysPkt_AssembleHardwareInfoReq    (CIFX_PACKET* ptPkt);

void    SysPkt_HandleFirmwareIdentifyCnf  (CIFX_PACKET* ptPkt);
void    SysPkt_HandleHardwareInfoCnf      (CIFX_PACKET* ptHardwareInfoCnf);

void    SysPkt_PD0InTriggeredReq          (CIFX_PACKET* ptPkt);

void    SysPkt_DeleteConfig               (CIFX_PACKET* ptPkt);

#endif /* NETX90_DPMCOMMUNICATION_INC_SYSTEMPACKETS_H_ */
