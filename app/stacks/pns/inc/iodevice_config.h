/*
 * iodevice_config.h
 *
 *  Created on: May 7, 2025
 *      Author: link
 */

#ifndef STACKS_PNS_INC_IODEVICE_CONFIG_H_
#define STACKS_PNS_INC_IODEVICE_CONFIG_H_

/*************************************************************************************
 Note: All features of the Profinet can enable in this central
       configuration file. Per default most of the are disabled.
*************************************************************************************/

/*************************************************************************************
 Set in Tag List the DDP to Passive. Enable this macro will send OEM Data like
 Serial Number, Order Number and Hardware Revison the firmware and set DDP to Active
**************************************************************************************/
//#define STM32_SETS_OEM_DATA

/*************************************************************************************
 Set in Tag List the DDP to Passive. Enable this macro will send MAC_ADDRESS
 and set DDP to Active
**************************************************************************************/
//#define STM32_SETS_MAC_ADDRESS

#endif /* STACKS_PNS_INC_IODEVICE_CONFIG_H_ */
