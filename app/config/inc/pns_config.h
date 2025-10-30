/*
 * pns_config.h
 *
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#ifndef INC_PNS_CONFIG_H_
#define INC_PNS_CONFIG_H_

/*************************************************************************************
 Enable one of the following macro shows the different use cases for IP address
 configuration.
*************************************************************************************/


/*************************************************************************************
 Set in Tag List the DDP to Passive. Enable this macro will send OEM Data like
 Serial Number, Order Number and Hardware Revison the firmware and set DDP to Active
**************************************************************************************/
//#define HOST_APPLICATION_SETS_OEM_DATA

/*************************************************************************************
 Set in Tag List the DDP to Passive. Enable this macro will send MAC_ADDRESS
 and set DDP to Active
**************************************************************************************/
//#define HOST_APPLICATION_SETS_MAC_ADDRESS

/*************************************************************************************
 HOST_APPLICATION_SETS_SERIAL_NUMBER

 Set in Tag List the DDP to Passive. Enable this macro will send OEM Data like
 Serial Number, Order Number and Hardware Revison the firmware and set DDP to Active
**************************************************************************************/
//#define HOST_APPLICATION_SETS_SERIAL_NUMBER

/*************************************************************************************
 HOST_ENABLE_BUS_SYNCHRONOUS

 Use the DIRQ Interrupt.
 If define is set, PDI0 Event is used as trigger to read, process and write the data

**************************************************************************************/
//#define HOST_ENABLE_BUS_SYNCHRONOUS

#endif /* INC_PNS_CONFIG_H_ */
