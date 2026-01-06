/*
 * system_defines.h
 *
 *  Created on: 05.01.2026
 *      Author: nnguyen
 */

#ifndef MANAGER_INC_APP_DEFINES_H_
#define MANAGER_INC_APP_DEFINES_H_

#define MSG_LED_QUEUE_LEN     1
#define MSG_LCD_QUEUE_LEN     1
#define MSG_NEOPXL_QUEUE_LEN  1

/**
 * @brief All available queues in the system
 */
typedef enum
{
  /* Management Queues */
//  QUEUE_ID_APP_MANAGER=0,          ///< Main application management queue
  /* Worker Queues */
  QUEUE_ID_LED_WORKER,           ///< LED control
  QUEUE_ID_LCD_WORKER,           ///< LCD display
  QUEUE_ID_NEOPIXEL_WORKER,      ///< Neopixel/LED strip control
  QUEUE_ID_COUNT,                ///< Total number of queues (MUST be last element!)
} MSG_QUEUE_ID_E;

/* AppManager Commandos */
//typedef enum {
//    APP_CMD_UPDATE_SIGNAL_LED = 0x10,
//    APP_CMD_UPDATE_LCD,
//    APP_CMD_UPDATE_NEOPIXEL_FROM_PLC,
//    APP_CMD_,
//    APP_CMD_SYSTEM_STANDBY,
//    APP_CMD_SYSTEM_STARTUP,
//    APP_CMD_FACTORY_RESET,
//    APP_CMD_UPDATE_NETWORK_CONFIG
//} eAppManagerCommand;

/* Debug function enable */
// #define DBG_ENABLE_LOGGING

#endif /* MANAGER_INC_APP_DEFINES_H_ */
