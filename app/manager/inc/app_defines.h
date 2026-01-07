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
#define MSG_APP_MAN_QUEUE_LEN 5

/**
 * @brief All available queues in the system
 */
typedef enum
{
  /* Management Queues */
  QUEUE_ID_APP_MANAGER=0,        ///< Main application management queue
  /* Worker Queues */
  QUEUE_ID_LED_WORKER,           ///< LED control
  QUEUE_ID_LCD_WORKER,           ///< LCD display
  QUEUE_ID_NEOPIXEL_WORKER,      ///< Neopixel/LED strip control
  QUEUE_ID_COUNT,                ///< Total number of queues (MUST be last element!)
} MSG_QUEUE_ID_E;

/* AppManager commands */
typedef enum
{
  APP_CMD_NETX_INIT_ERR = 0x10,
  APP_CMD_NETX_INIT_OK,
  APP_CMD_NETX_CONFIG_ERR,
  APP_CMD_NETX_CONFIG_OK,
  APP_CMD_NETX_DPM_ERR,
  APP_CMD_NETX_PLC_CONNECTED,
  APP_CMD_NETX_PLC_DISCONNECTED,
  APP_CMD_NETX_UPDATE_IODATA,
  APP_CMD_NETX_GENERAL_ERR,
  APP_CMD_COUNT
} APP_COMMAND_E;

//typedef struct
//{
//  APP_COMMAND_E eCommand;
//  void *pvData;
//} APP_MESSAGE_T;

/* Debug function enable */
// #define DBG_ENABLE_LOGGING
void AppManager_SendCommand(APP_COMMAND_E eMessage);

#endif /* MANAGER_INC_APP_DEFINES_H_ */
