/**************************************************************************************
Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.
***************************************************************************************
$Id: pns_Results.h 107692 2022-07-05 12:25:40Z BMeyer $:

Description:
  Profinet Stack status codes
**************************************************************************************/

#ifndef PNS_RESULTS_H_
#define PNS_RESULTS_H_

#include<stdint.h>

/***********************************************************************************/
/* PROFINET IO-Device Interface Task */
/***********************************************************************************/
/** Invalid command. */
#define ERR_PNS_IF_COMMAND_INVALID       0xC0300001L

/** Initialization of PNS Operating system adaption failed. */
#define ERR_PNS_IF_OS_INIT_FAILED        0xC0300002L

/** Initialization of PNS IP address failed. */
#define ERR_PNS_IF_SET_INIT_IP_FAILED    0xC0300003L

/** PROFINET IO-Device Setup failed. */
#define ERR_PNS_IF_PNIO_SETUP_FAILED     0xC0300004L

/** Device information set already. */
#define ERR_PNS_IF_DEVICE_INFO_ALREADY_SET 0xC0300005L

/** Setting of device information failed. */
#define ERR_PNS_IF_SET_DEVICE_INFO_FAILED 0xC0300006L

/** PROFINET IO-Device stack is not initialized. Send PNS_IF_SET_DEVICEINFO_REQ before PNS_IF_OPEN_DEVICE_REQ. */
#define ERR_PNS_IF_NO_DEVICE_SETUP       0xC0300007L

/** Opening a device instance failed. */
#define ERR_PNS_IF_DEVICE_OPEN_FAILED    0xC0300008L

/** No device instance open. */
#define ERR_PNS_IF_NO_DEVICE_INSTANCE    0xC0300009L

/** Plugging a module failed. */
#define ERR_PNS_IF_PLUG_MODULE_FAILED    0xC030000AL

/** Plugging a submodule failed. */
#define ERR_PNS_IF_PLUG_SUBMODULE_FAILED 0xC030000BL

/** Start of PROFINET IO-Device failed. */
#define ERR_PNS_IF_DEVICE_START_FAILED   0xC030000CL

/** Start of network communication failed. */
#define ERR_PNS_IF_EDD_ENABLE_FAILED     0xC030000DL

/** Allocation of a device instance management buffer failed. */
#define ERR_PNS_IF_ALLOC_MNGMNT_BUFFER_FAILED 0xC030000EL

/** Given device handle is NULL. */
#define ERR_PNS_IF_DEVICE_HANDLE_NULL    0xC030000FL

/** Command PNS_IF_SET_APPL_READY_REQ failed. */
#define ERR_PNS_IF_SET_APPL_READY_FAILED 0xC0300010L

/** Command PNS_IF_SET_DEVSTATE_REQ failed. */
#define ERR_PNS_IF_SET_DEVSTATE_FAILED   0xC0300011L

/** Pulling the submodule failed. */
#define ERR_PNS_IF_PULL_SUBMODULE_FAILED 0xC0300012L

/** Pulling the module failed. */
#define ERR_PNS_IF_PULL_MODULE_FAILED    0xC0300013L

/** Destination ID in command invalid. */
#define ERR_PNS_IF_WRONG_DEST_ID         0xC0300014L

/** Device Handle in command invalid. */
#define ERR_PNS_IF_DEVICE_HANDLE_INVALID 0xC0300015L

/** PNS stack callback timeout. */
#define ERR_PNS_IF_CALLBACK_TIMEOUT      0xC0300016L

/** PNS_IF packet pool empty. */
#define ERR_PNS_IF_PACKET_POOL_EMPTY     0xC0300017L

/** Command PNS_IF_ADD_API_REQ failed. */
#define ERR_PNS_IF_ADD_API_FAILED        0xC0300018L

/** Setting submodule state failed. */
#define ERR_PNS_IF_SET_SUB_STATE_FAILED  0xC0300019L

/** No network configuration DBM-file. */
#define ERR_PNS_IF_NO_NW_DBM_ERROR       0xC030001AL

/** Error during reading the "SETUP" table of the network configuration DBM-file . */
#define ERR_PNS_IF_NW_SETUP_TABLE_ERROR  0xC030001BL

/** Error during reading the "SETUP" table of the config.xxx DBM-file . */
#define ERR_PNS_IF_CFG_SETUP_TABLE_ERROR 0xC030001CL

/** No config.xxx DBM-file. */
#define ERR_PNS_IF_NO_CFG_DBM_ERROR      0xC030001DL

/** Error getting dataset pointer. */
#define ERR_PNS_IF_DBM_DATASET_ERROR     0xC030001EL

/** Error getting dataset pointer(SETUP_EX table). */
#define ERR_PNS_IF_SETUPEX_TABLE_ERROR   0xC030001FL

/** Error getting either dataset pointer or number of datasets(AP table). */
#define ERR_PNS_IF_AP_TABLE_ERROR        0xC0300020L

/** Error getting either dataset pointer or number of datasets(MODULE table). */
#define ERR_PNS_IF_MODULES_TABLE_ERROR   0xC0300021L

/** Error getting either dataset pointer or number of datasets(SUBMODULE table). */
#define ERR_PNS_IF_SUBMODULES_TABLE_ERROR 0xC0300022L

/** Error setting up PNIO configuration(PNIO_setup()). */
#define ERR_PNS_IF_PNIO_SETUP_ERROR      0xC0300023L

/** Error getting record of "MODULES" linked table. */
#define ERR_PNS_IF_MODULES_GET_REC       0xC0300024L

/** Error getting record of "SUBMODULES" linked table. */
#define ERR_PNS_IF_SUBMODULES_GET_REC    0xC0300025L

/** Error accessing "PNIOD_MODULE_ID" table or table record error. */
#define ERR_PNS_IF_PNIOD_MODULE_ID_TABLE_ERROR 0xC0300026L

/** Error accessing "SIGNALS" table or table record error. */
#define ERR_PNS_IF_SIGNALS_TABLE_ERROR   0xC0300027L

/** Error accessing "MODULES_IO" table or table record error. */
#define ERR_PNS_IF_MODULES_IO_TABLE_ERROR 0xC0300028L

/** Error accessing "CHANNEL_SETTING" table or table record error. */
#define ERR_PNS_IF_CHANNEL_SETTING_TABLE_ERROR 0xC0300029L

/** Error writing DBM-file. */
#define ERR_PNS_IF_WRITE_DBM             0xC030002AL

/** No basic DPM configuration. */
#define ERR_PNS_IF_DPM_CONFIG            0xC030002BL

/** Application did not trigger the watchdog. */
#define ERR_PNS_IF_WATCHDOG              0xC030002CL

/** Data length in "SIGNALS" table does not correspond to that in "SUBMODULES" table. */
#define ERR_PNS_IF_SIGNALS_SUBMODULES    0xC030002DL

/** Failed to read DPM subarea. */
#define ERR_PNS_IF_READ_DPM_SUBAREA      0xC030002EL

/** Error configuring Module 0 Submodule 1. */
#define ERR_PNS_IF_MOD_0_SUB_1           0xC030002FL

/** Length of I/O signals is bigger than the size of DPM subarea. */
#define ERR_PNS_IF_SIGNALS_LENGTH        0xC0300030L

/** A submodule cannot have input and outputs at the same time. */
#define ERR_PNS_IF_SUB_TRANSFER_DIRECTION 0xC0300031L

/** Error while formatting PNVOLUME. */
#define ERR_PNS_IF_FORMAT_PNVOLUME       0xC0300032L

/** Error while mounting PNVOLUME. */
#define ERR_PNS_IF_MOUNT_PNVOLUME        0xC0300033L

/** Error during initialization of the remote resources of the stack. */
#define ERR_PNS_IF_INIT_REMOTE           0xC0300034L

/** Warmstart parameters are redundant. The stack was configured with DBM or packets. */
#define ERR_PNS_IF_WARMSTART_CONFIG_REDUNDANT 0xC0300035L

/** Incorrect warmstart parameter(s). */
#define ERR_PNS_IF_WARMSTART_PARAMETER   0xC0300036L

/** PNIO_set_appl_state_ready() returns error. */
#define ERR_PNS_IF_SET_APPL_STATE_READY  0xC0300037L

/** PNIO_set_dev_state() returns error. */
#define ERR_PNS_IF_SET_DEV_STATE         0xC0300038L

/** PNIO_process_alarm_send() returns error. */
#define ERR_PNS_IF_PROCESS_ALARM_SEND    0xC0300039L

/** PNIO_ret_of_sub_alarm_send() returns error. */
#define ERR_PNS_IF_RET_OF_SUB_ALARM_SEND 0xC030003AL

/** PNIO_diag_alarm_send() returns error. */
#define ERR_PNS_IF_DIAG_ALARM_SEND       0xC030003BL

/** PNIO_diag_generic_add() returns error. */
#define ERR_PNS_IF_DIAG_GENERIC_ADD      0xC030003CL

/** PNIO_diag_generic_remove() returns error. */
#define ERR_PNS_IF_DIAG_GENERIC_REMOVE   0xC030003DL

/** PNIO_diag_channel_add() returns error. */
#define ERR_PNS_IF_DIAG_CHANNEL_ADD      0xC030003EL

/** PNIO_diag_channel_remove() returns error. */
#define ERR_PNS_IF_DIAG_CHANNEL_REMOVE   0xC030003FL

/** PNIO_ext_diag_channel_add() returns error. */
#define ERR_PNS_IF_EXT_DIAG_CHANNEL_ADD  0xC0300040L

/** PNIO_ext_diag_channel_remove() returns error. */
#define ERR_PNS_IF_EXT_DIAG_CHANNEL_REMOVE 0xC0300041L

/** Parameter station name length is incorrect. */
#define ERR_PNS_IF_STATION_NAME_LEN      0xC0300042L

/** Parameter station name is incorrect. */
#define ERR_PNS_IF_STATION_NAME          0xC0300043L

/** Parameter station type length is incorrect. */
#define ERR_PNS_IF_STATION_TYPE_LEN      0xC0300044L

/** Parameter device type is incorrect. */
#define ERR_PNS_IF_DEVICE_TYPE           0xC0300045L

/** Parameter order id is incorrect. */
#define ERR_PNS_IF_ORDER_ID              0xC0300046L

/** Parameter input data status bytes length is incorrect. */
#define ERR_PNS_IF_INPUT_STATUS          0xC0300047L

/** Parameter output data status bytes length is incorrect. */
#define ERR_PNS_IF_OUTPUT_STATUS         0xC0300048L

/** Parameter watchdog timing is incorrect(must be >= 10). */
#define ERR_PNS_IF_WATCHDOG_PARAMETER    0xC0300049L

/** Parameter output data update timing is incorrect. */
#define ERR_PNS_IF_OUT_UPDATE            0xC030004AL

/** Parameter input data update timing is incorrect. */
#define ERR_PNS_IF_IN_UPDATE             0xC030004BL

/** Parameter input memory area size is incorrect. */
#define ERR_PNS_IF_IN_SIZE               0xC030004CL

/** Parameter output memory area size is incorrect. */
#define ERR_PNS_IF_OUT_SIZE              0xC030004DL

/** Unable to allocate memory for global access to local resources. */
#define ERR_PNS_IF_GLOBAL_RESOURCES      0xC030004EL

/** Unable to allocate memory for dynamic configuration packet. */
#define ERR_PNS_IF_DYNAMIC_CFG_PCK       0xC030004FL

/** Unable to stop device. */
#define ERR_PNS_IF_DEVICE_STOP           0xC0300050L

/** Parameter device id is incorrect. */
#define ERR_PNS_IF_DEVICE_ID             0xC0300051L

/** Parameter vendor id is incorrect. */
#define ERR_PNS_IF_VENDOR_ID             0xC0300052L

/** Parameter system start is incorrect. */
#define ERR_PNS_IF_SYS_START             0xC0300053L

/** The length of IO data expected by the controller exceeds the limit specified in warmstart parameters. */
#define ERR_PNS_IF_DYN_CFG_IO_LENGTH     0xC0300054L

/** The count of the IO modules expected by the controller exceeds the supported by the stack count. */
#define ERR_PNS_IF_DYN_CFG_MOD_NUM       0xC0300055L

/** No global access to local resources. */
#define ERR_PNS_IF_ACCESS_LOCAL_RSC      0xC0300056L

/** Plugging and pulling modules during creation of communication is not allowed. */
#define ERR_PNS_IF_PULL_PLUG             0xC0300057L

/** Maximum number of ARs is 1. */
#define ERR_PNS_IF_AR_NUM                0xC0300058L

/** Only API = 0 is supported. */
#define ERR_PNS_IF_API_NUM               0xC0300059L

/** Device is already opened. */
#define ERR_PNS_IF_ALREADY_OPEN          0xC030005AL

/** Application is already added. */
#define ERR_PNS_IF_API_ADDED             0xC030005BL

/** Configuration modes should not be mixed( DBM-files,application,warmstart message). */
#define ERR_PNS_IF_CONFIG_MODE           0xC030005CL

/** Unknown LED mode. */
#define ERR_PNS_IF_UNK_LED_MODE          0xC030005DL

/** Physical link rate is less than 100 Mbit. */
#define ERR_PNS_IF_PHYSICAL_LINK         0xC030005EL

/** Number of slots or subslots too big. */
#define ERR_PNS_IF_MAX_SLOT_SUBSLOT      0xC030005FL

/** AR error. Out of memory. */
#define ERR_PNS_IF_AR_REASON_MEM         0xC0300060L

/** AR error. Add provider or consumer failed. */
#define ERR_PNS_IF_AR_REASON_FRAME       0xC0300061L

/** AR error. Consumer missing. */
#define ERR_PNS_IF_AR_REASON_MISS        0xC0300062L

/** AR error. CMI timeout. */
#define ERR_PNS_IF_AR_REASON_TIMER       0xC0300063L

/** AR error. Alarm open failed. */
#define ERR_PNS_IF_AR_REASON_ALARM       0xC0300064L

/** AR error. Alarm send confirmation failed. */
#define ERR_PNS_IF_AR_REASON_ALSND       0xC0300065L

/** AR error. Alarm acknowledge send confirmation failed. */
#define ERR_PNS_IF_AR_REASON_ALACK       0xC0300066L

/** AR error. Alarm data too long. */
#define ERR_PNS_IF_AR_REASON_ALLEN       0xC0300067L

/** AR error. Alarm indication error. */
#define ERR_PNS_IF_AR_REASON_ASRT        0xC0300068L

/** AR error. RPC client call confirmation failed. */
#define ERR_PNS_IF_AR_REASON_RPC         0xC0300069L

/** AR error. Abort request. */
#define ERR_PNS_IF_AR_REASON_ABORT       0xC030006AL

/** AR error. Re-Run. */
#define ERR_PNS_IF_AR_REASON_RERUN       0xC030006BL

/** AR error. Release indication received. */
#define ERR_PNS_IF_AR_REASON_REL         0xC030006CL

/** AR error. Device deactivated. */
#define ERR_PNS_IF_AR_REASON_PAS         0xC030006DL

/** AR error. Device/ar removed. */
#define ERR_PNS_IF_AR_REASON_RMV         0xC030006EL

/** AR error. Protocol violation. */
#define ERR_PNS_IF_AR_REASON_PROT        0xC030006FL

/** AR error. NARE error. */
#define ERR_PNS_IF_AR_REASON_NARE        0xC0300070L

/** AR error. RPC-Bind error. */
#define ERR_PNS_IF_AR_REASON_BIND        0xC0300071L

/** AR error. RPC-Connect error. */
#define ERR_PNS_IF_AR_REASON_CONNECT     0xC0300072L

/** AR error. RPC-Read error. */
#define ERR_PNS_IF_AR_REASON_READ        0xC0300073L

/** AR error. RPC-Write error. */
#define ERR_PNS_IF_AR_REASON_WRITE       0xC0300074L

/** AR error. RPC-Control error. */
#define ERR_PNS_IF_AR_REASON_CONTROL     0xC0300075L

/** AR error. Unknown. */
#define ERR_PNS_IF_AR_REASON_UNKNOWN     0xC0300076L

/** Watchdog initialization failed. */
#define ERR_PNS_IF_INIT_WATCHDOG         0xC0300077L

/** The Device is not connected to a network. */
#define ERR_PNS_IF_NO_PHYSICAL_LINK      0xC0300078L

/** Failed to copy from DPM or to DPM the cyclic IO data. */
#define ERR_PNS_IF_DPM_CYCLIC_IO_RW      0xC0300079L

/** The submodule does not exist. */
#define ERR_PNS_IF_SUBMODULE             0xC030007AL

/** The module does not exist. */
#define ERR_PNS_IF_MODULE                0xC030007BL

/** The AR was closed or the AR handle is not valid. */
#define ERR_PNS_IF_NO_AR                 0xC030007CL

/** Timeout while waiting for response to write_record_indication. */
#define ERR_PNS_IF_WRITE_REC_RES_TIMEOUT 0xC030007DL

/** The sender of the request in not registered with request PNS_IF_REGISTER_AP_REQ. */
#define ERR_PNS_IF_UNREGISTERED_SENDER   0xC030007EL

/** Unknown record handle. */
#define ERR_PNS_IF_RECORD_HANDLE_INVALID 0xC030007FL

/** Another instance is registered at the moment. */
#define ERR_PNS_IF_REGISTER_AP           0xC0300080L

/** One instance cannot unregister another one. */
#define ERR_PNS_IF_UNREGISTER_AP         0xC0300081L

/** The Must-configuration differs from the Is-configuration. */
#define ERR_PNS_IF_CONFIG_DIFFER         0xC0300082L

/** No communication processing. */
#define ERR_PNS_IF_NO_COMMUNICATION      0xC0300083L

/** At least one parameter in a packet was wrong or/and did not meet the requirements. */
#define ERR_PNS_IF_BAD_PARAMETER         0xC0300084L

/** Input or Output data requires more space than available. */
#define ERR_PNS_IF_AREA_OVERFLOW         0xC0300085L

/** Saving Warmstart Configuration for later use was not successful. */
#define ERR_PNS_IF_WRM_PCK_SAVE          0xC0300086L

/** AR error. Pull and Plug are forbidden after check.rsp and before in-data.ind. */
#define ERR_PNS_IF_AR_REASON_PULLPLUG    0xC0300087L

/** AR error. AP has been removed. */
#define ERR_PNS_IF_AR_REASON_AP_RMV      0xC0300088L

/** AR error. Link "down". */
#define ERR_PNS_IF_AR_REASON_LNK_DWN     0xC0300089L

/** AR error. Could not register multicast-MAC. */
#define ERR_PNS_IF_AR_REASON_MMAC        0xC030008AL

/** AR error. Not synchronized (Cannot start companion-AR). */
#define ERR_PNS_IF_AR_REASON_SYNC        0xC030008BL

/** AR error. Wrong topology(Cannot start companion-AR). */
#define ERR_PNS_IF_AR_REASON_TOPO        0xC030008CL

/** AR error. DCP. Station Name changed. */
#define ERR_PNS_IF_AR_REASON_DCP_NAME    0xC030008DL

/** AR error. DCP. Reset to factory-settings. */
#define ERR_PNS_IF_AR_REASON_DCP_RESET   0xC030008EL

/** AR error. Cannot start companion-AR because a 0x8ipp submodule in the first AR /has appl-ready-pending/ is locked/ is wrong or pulled/ . */
#define ERR_PNS_IF_AR_REASON_PRM         0xC030008FL

/** Packet management error. */
#define ERR_PNS_IF_PACKET_MNGMNT         0xC0300090L

/** Invalid parameter API. */
#define ERR_PNS_IF_WRONG_API_NUM         0xC0300091L

/** Invalid parameter ModuleIdentifier (a module with different ModuleIdentifier is already plugged). */
#define ERR_PNS_IF_WRONG_MODULE_ID       0xC0300092L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_WRONG_MODULE_NUM      0xC0300093L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_UNS_AREA              0xC0300094L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_WRONG_SUB_ID          0xC0300095L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_WRONG_SUBMODULE_NUM   0xC0300096L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_DEVICE_STOP_FAILED    0xC0300097L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_EDD_DISABLE_FAILED    0xC0300098L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_WRITE_IN              0xC0300099L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_READ_OUT              0xC030009AL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_PNIO_STATUS           0xC030009BL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_WRONG_MODULE_ADDRESS  0xC030009CL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_UNK_DEVICE_STATE      0xC030009DL

/** Invalid alarm data length. */
#define ERR_PNS_IF_ALARM_DATA_LEN        0xC030009EL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_UNK_SUBMODULE_STATE   0xC030009FL

/** Invalid parameter Diagnosis handle. */
#define ERR_PNS_IF_BAD_DIAG_HANDLE       0xC03000A0L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_UNS_STRUCT_ID         0xC03000A1L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_UNK_ALARM_STATE       0xC03000A2L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_DIAG_DATA_LEN         0xC03000A3L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_BAD_CHANNEL_ERR_TYPE  0xC03000A4L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_BAD_CHANNEL_PROP      0xC03000A5L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_BAD_CHANNEL_NUM       0xC03000A6L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_RCX_RESTART           0xC03000A7L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_CFG_MNGMNT            0xC03000A8L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_UNK_INTERN_REQ        0xC03000A9L

/** Obsolete, no longer used. */
#define ERR_PNS_IF_CFG_STORE             0xC03000AAL

/** An internal error occurred while deleting the configuration. */
#define ERR_PNS_IF_CFG_DELETE_FAILED     0xC03000ABL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_READ_CFG              0xC03000ACL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_ACCESS_SYS_VOLUME     0xC03000ADL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_ACCESS_BCKUP_VOLUME   0xC03000AEL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_CFG_BAD_LEN           0xC03000AFL

/** Obsolete, no longer used. */
#define ERR_PNS_IF_WRM_CFG_MNGMNT        0xC03000B0L

/** No registered application. Reset_to_factory_settings Indication failed. */
#define ERR_PNS_IF_RESET_FACTORY_IND     0xC03000B1L

/** A module was already plugged to the slot. */
#define ERR_PNS_IF_MODULE_ALREADY_PLUGGED 0xC03000B2L

/** Failed to init the OS adaptation layer. */
#define ERR_PNS_IF_OSINIT                0xC03000B3L

/** Failed to init the TCPIP adaptation layer. */
#define ERR_PNS_IF_OSSOCKINIT            0xC03000B4L

/** Invalid subnetwork mask. */
#define ERR_PNS_IF_INVALID_NETMASK       0xC03000B5L

/** Invalid IP address. */
#define ERR_PNS_IF_INVALID_IP_ADDR       0xC03000B6L

/** Erroneous Task start-up parameters. */
#define ERR_PNS_IF_STA_STARTUP_PARAMETER 0xC03000B7L

/** Failed to initialize the Task local resources. */
#define ERR_PNS_IF_INIT_LOCAL            0xC03000B8L

/** The configuration per packets is incomplete. */
#define ERR_PNS_IF_APP_CONFIG_INCOMPLETE 0xC03000B9L

/** EDD Initialization failed. */
#define ERR_PNS_IF_INIT_EDD              0xC03000BAL

/** DPM is not enabled. */
#define ERR_PNS_IF_DPM_NOT_ENABLED       0xC03000BBL

/** Reading Link Status failed. */
#define ERR_PNS_IF_READ_LINK_STATUS      0xC03000BCL

/** Invalid gateway address (not reachable with configured netmask). */
#define ERR_PNS_IF_INVALID_GATEWAY       0xC03000BDL

/* these #defines are from Benjamin - for use with new PNIO stack (none-Siemens) */
/** Error while sending a packet to another task. */
#define ERR_PNS_IF_PACKET_SEND_FAILED    0xC0300100L

/** Insufficient memory to handle the request. */
#define ERR_PNS_IF_RESOURCE_OUT_OF_MEMORY 0xC0300101L

/** No application to send the indication to is registered. */
#define ERR_PNS_IF_NO_APPLICATION_REGISTERED 0xC0300102L

/** The host-application returned a packet with invalid (changed) SourceID. */
#define ERR_PNS_IF_INVALID_SOURCE_ID     0xC0300103L

/** The buffer used to store packets exchanged between host-application and stack is full. */
#define ERR_PNS_IF_PACKET_BUFFER_FULL    0xC0300104L

/** Pulling the (sub)module failed because no module is plugged into the slot specified. */
#define ERR_PNS_IF_PULL_NO_MODULE        0xC0300105L

/** Pulling the submodule failed because no submodule is plugged into the subslot specified. */
#define ERR_PNS_IF_PULL_NO_SUBMODULE     0xC0300106L

/** The packet buffer storing packets exchanged between host-application and stack returned an invalid packet. */
#define ERR_PNS_IF_PACKET_BUFFER_RESTORE_ERROR 0xC0300107L

/** Diagnosis data not accepted because no module is plugged into the slot specified. */
#define ERR_PNS_IF_DIAG_NO_MODULE        0xC0300108L

/** Diagnosis data not accepted because no submodule is plugged into the subslot specified. */
#define ERR_PNS_IF_DIAG_NO_SUBMODULE     0xC0300109L

/** The services requested are not available while cyclic communication is running. */
#define ERR_PNS_IF_CYCLIC_EXCHANGE_ACTIVE 0xC030010AL

/** This fatal error callback function could not be registered because there is already a function registered. */
#define ERR_PNS_IF_FATAL_ERROR_CLB_ALREADY_REGISTERED 0xC030010BL

/** The stack did not accept the warmstart parameters. */
#define ERR_PNS_IF_ERROR_STACK_WARMSTART_CONFIGURATION 0xC030010CL

/** The stack did not accept the module configuration packet. */
#define ERR_PNS_IF_ERROR_STACK_MODULE_CONFIGURATION 0xC030010DL

/** The stack sent a Check Indication for an unexpected module. This module was not part of the CR Info Indication. */
#define ERR_PNS_IF_CHECK_IND_FOR_UNEXPECTED_MODULE 0xC030010EL

/** The stack sent a Check Indication for an unexpected submodule. This submodule was not part of the CR Info Indication. */
#define ERR_PNS_IF_CHECK_IND_FOR_UNEXPECTED_SUBMODULE 0xC030010FL

/** No more diagnosis records can be added to the stack because the maximum amount is already reached. */
#define ERR_PNS_DIAG_BUFFER_FULL         0xC0300110L

/** The stack sent a Check Indication for an unexpected API. This API was not part of the CR Info Indication. */
#define ERR_PNS_IF_CHECK_IND_FOR_UNEXPECTED_API 0xC0300111L

/** The DPM shall be accessed with an invalid data offset. */
#define ERR_PNS_IF_DPM_ACCESS_WITH_INVALID_OFFSET 0xC0300112L

/** The stack indicated to CR Info Indications with type input. */
#define ERR_PNS_IF_DUPLICATE_INPUT_CR_INFO 0xC0300113L

/** The stack indicated to CR Info Indications with type output. */
#define ERR_PNS_IF_DUPLICATE_OUTPUT_CR_INFO 0xC0300114L

/** The stack indicated a faulty CR Info Indications. */
#define ERR_PNS_IF_FAULTY_CR_INFO_IND_RECEIVED 0xC0300115L

/** The request cannot be executed because configuration reload or ChannelInit is running. */
#define ERR_PNS_IF_CONFIG_RELOAD_RUNNING 0xC0300116L

/** There is no valid chassis MAC address set Without MAC address the stack will not work. */
#define ERR_PNS_IF_NO_MAC_ADDRESS_SET    0xC0300117L

/** The Port MAC addresses have to be set before sending Set-Configuration Request to the stack. */
#define ERR_PNS_IF_SET_PORT_MAC_NOT_POSSIBLE 0xC0300118L

/** Evaluating the module configuration failed. */
#define ERR_PNS_IF_INVALID_MODULE_CONFIGURATION 0xC030011AL

/** The sum of IO-data length exceeds the maximum allowed value. */
#define ERR_PNS_IF_CONF_IO_LEN_TO_BIG    0xC030011BL

/** The module configuration does not contain at least one module. */
#define ERR_PNS_IF_NO_MODULE_CONFIGURED  0xC030011CL

/** The value of bSwRevisionPrefix is invalid. */
#define ERR_PNS_IF_INVALID_SW_REV_PREFIX 0xC030011DL

/** The value of usReserved it not zero. */
#define ERR_PNS_IF_RESERVED_VALUE_NOT_ZERO 0xC030011EL

/** Identifying the stack message queue CMDEV failed. */
#define ERR_PNS_IF_IDENTIFY_CMDEV_QUEUE_FAILED 0xC030011FL

/** Creating the sync message queue failed. */
#define ERR_PNS_IF_CREATE_SYNC_QUEUE_FAILED 0xC0300120L

/** Creating the low alarm message queue failed. */
#define ERR_PNS_IF_CREATE_ALARM_LOW_QUEUE_FAILED 0xC0300121L

/** Creating the high alarm message queue failed. */
#define ERR_PNS_IF_CREATE_ALARM_HIGH_QUEUE_FAILED 0xC0300122L

/** While evaluating SetConfiguration packet the packet length was found smaller than amount of configured modules needs. */
#define ERR_PNS_IF_CFG_PACKET_TO_SMALL   0xC0300123L

/** A fatal error occurred prior to this request. Therefore this request cannot be fulfilled. */
#define ERR_PNS_IF_FATAL_ERROR_OCCURRED  0xC0300124L

/** The request cannot be executed because the submodule is not in cyclic data exchange. */
#define ERR_PNS_IF_SUBMODULE_NOT_IN_CYCLIC_EXCHANGE 0xC0300125L

/** This service is not available through DPM. */
#define ERR_PNS_IF_SERVICE_NOT_AVAILABLE_THROUGH_DPM 0xC0300126L

/** The version of parameters is invalid (most likely too old). */
#define ERR_PNS_IF_INVALID_PARAMETER_VERSION 0xC0300127L

/** The usage of database is forbidden by startup parameters of task. */
#define ERR_PNS_IF_DATABASE_USAGE_IS_FORBIDDEN 0xC0300128L

/** The amount of record data is too big. */
#define ERR_PNS_IF_RECORD_LENGTH_TOO_BIG 0xC0300129L

/** Identifying the stack message queue LLDP failed. */
#define ERR_PNS_IF_IDENTIFY_LLDP_QUEUE_FAILED 0xC030012AL

/** SetConfiguration Requests total packet length is invalid. */
#define ERR_PNS_IF_INVALID_TOTAL_PACKET_LENGTH 0xC030012BL

/** The application needed too much time to respond to an indication. */
#define ERR_PNS_IF_APPLICATION_TIMEOUT   0xC030012CL

/** The packet buffer storing packets exchanged between host-application and stack returned a faulty packet. */
#define ERR_PNS_IF_PACKET_BUFFER_INVALID_PACKET 0xC030012DL

/** The request cannot be handled until a valid IO Image configuration is available. */
#define ERR_PNS_IF_NO_IO_IMAGE_CONFIGURATION_AVAILABLE 0xC030012EL

/** A valid IO Image configuration is already available. */
#define ERR_PNS_IF_IO_IMAGE_ALREADY_CONFIGURED 0xC030012FL

/** A submodule may only be plugged into a PDEV-subslot which does not exceed the number of supported interfaces and port numbers. */
#define ERR_PNS_IF_INVALID_PDEV_SUBSLOT  0xC0300130L

/** The module configuration does not contain a Device Access Point DAP-submodule in slot 0 subslot 1. */
#define ERR_PNS_IF_NO_DAP_PRESENT        0xC0300131L

/** Output size of the submodule exceeded. Configured value of ulCompleteOutputSize is smaller than the Output size of all plugged input modules. Upgrade ulCompleteOutputSize. */
#define ERR_PNS_IF_PLUG_SUBMOD_OUTPUT_SIZE_EXCEEDED 0xC0300132L

/** Input size of the submodule exceeded. Configured value of ulCompleteInputSize is smaller than the Input size of all plugged input modules. Upgrade ulCompleteInputSize. */
#define ERR_PNS_IF_PLUG_SUBMOD_INPUT_SIZE_EXCEEDED 0xC0300133L

/** No module attached to add the submodule to. */
#define ERR_PNS_IF_PLUG_SUBMOD_NO_MODULE_ATTACHED_TO_ADD_TO 0xC0300134L

/** Submodule already plugged. */
#define ERR_PNS_IF_PLUG_SUBMOD_ALREADY_PLUGGED_THIS_SUBMOD 0xC0300135L

/** Invalid IOXS provider image. */
#define ERR_PNS_IF_SETIOXS_INVALID_PROV_IMAGE 0xC0300136L

/** Invalid IOXS consumer image. */
#define ERR_PNS_IF_SETIOXS_INVALID_CONS_IMAGE 0xC0300137L

/** Invalid IOPS mode. */
#define ERR_PNS_IF_INVALID_IOPS_MODE     0xC0300138L

/** Invalid IOCS mode. */
#define ERR_PNS_IF_INVALID_IOCS_MODE     0xC0300139L

/** Invalid API. */
#define ERR_PNS_IF_INVALID_API           0xC030013AL

/** Invalid slot. */
#define ERR_PNS_IF_INVALID_SLOT          0xC030013BL

/** Invalid subslot. */
#define ERR_PNS_IF_INVALID_SUBSLOT       0xC030013CL

/** Invalid channel number. */
#define ERR_PNS_IF_INVALID_CHANNEL_NUMBER 0xC030013DL

/** Invalid channel properties. */
#define ERR_PNS_IF_INVALID_CHANNEL_PROPERTIES 0xC030013EL

/** Invalid channel error type not allowed. */
#define ERR_PNS_IF_CHANNEL_ERRORTYPE_NOT_ALLOWED 0xC030013FL

/** Invalid channel EXT error type not allowed. */
#define ERR_PNS_IF_EXT_CHANNEL_ERRORTYPE_NOT_ALLOWED 0xC0300140L

/** Invalid user struct identifier. */
#define ERR_PNS_IF_INVALID_USER_STRUCT_IDENTIFIER 0xC0300141L

/** Invalid submodule. */
#define ERR_PNS_IF_INVALID_SUBMODULE     0xC0300142L

/** Invalid IM type. */
#define ERR_PNS_IF_INVALID_IM_TYPE       0xC0300143L

/** Failed to identify the FODMI Queue. */
#define ERR_PNS_IF_IDENTIFY_FODMI_QUEUE_FAILED 0xC0300144L

/** The DPM Receive Mailbox Queue run out of space. Most likely the host did not fetch the packets. */
#define ERR_PNS_IF_DPM_MAILBOX_OVERFLOW  0xC0300145L

/** The application denied read/write access to I&M record object. */
#define ERR_PNS_IF_APPL_IM_ACCESS_DENIED 0xC0300146L

/** The application does not implement the requested I&M record object. */
#define ERR_PNS_IF_APPL_IM_INVALID_INDEX 0xC0300147L

/** Invalid number of max supported submodules. */
#define ERR_PNS_IF_TAGLIST_INVALID_SUBMODULE_NUMBER 0xC0300148L

/** Invalid number of max supported additional IO ARs. */
#define ERR_PNS_IF_TAGLIST_INVALID_ADDITIONAL_AR_NUMBER 0xC0300149L

/** Invalid number of max supported implicit IO ARs. */
#define ERR_PNS_IF_TAGLIST_INVALID_IMPLICIT_AR_NUMBER 0xC030014AL

/** Invalid number of max supported Device Access ARs. */
#define ERR_PNS_IF_TAGLIST_INVALID_DAAR_NUMBER 0xC030014BL

/** Invalid RPC buffer size. */
#define ERR_PNS_IF_TAGLIST_INVALID_MIN_RPC_BUFFER_SIZE 0xC030014CL

/** Invalid number of max supported diagnosis entries . */
#define ERR_PNS_IF_TAGLIST_INVALID_DIAGNOSIS_ENTRIES_NUM 0xC030014DL

/** Invalid number of max supported AR sets. */
#define ERR_PNS_IF_TAGLIST_INVALID_ARSET_NUM 0xC030014EL

/** A PE Entity was already added to this submodule. */
#define ERR_PNS_IF_PE_ENTITY_EXISTS      0xC030014FL

/** The submodule does not have a PE Entity. */
#define ERR_PNS_IF_NO_PE_ENTITY          0xC0300150L

/** The hardware revision field was set to an invalid value. */
#define ERR_PNS_IF_INVALID_HW_REVISION   0xC0300151L

/** The serial number field was set to an invalid value. */
#define ERR_PNS_IF_INVALID_SERIAL_NUMBER 0xC0300152L

/** The dataprovider OEM fields are incomplete regarding Profinet protocol. */
#define ERR_PNS_IF_INCONSISTENT_DDP_CONTENT 0xC0300153L

/** Configuration fault while evaluating database. */
#define ERR_PNS_IF_CONFIG_FAILURE_TLV_DATABASE 0xC0300154L

/** The set configuration service system flags remanent data settings conflicts with firmware taglist remanent data settings. */
#define ERR_PNS_IF_INCONSISTENT_REMANENT_HANDLING 0xC0300155L

/* START: On 12.11.08 Konstantin added: Area for Abort- and Offline- Reason error codes to support Siemens Stack */
/* Message IDs 0x0A00 - 0x0AFF */
/** None. Unused. */
#define ERR_PNS_IF_CM_AR_REASON_NONE     0xC0300A00L

/** This error code is obsolete. */


/** This error code is obsolete. */


/** AR Out of memory. */
#define ERR_PNS_IF_CM_AR_REASON_MEM      0xC0300A03L

/** AR add provider or consumer failed. */
#define ERR_PNS_IF_CM_AR_REASON_FRAME    0xC0300A04L

/** AR consumer DHT/WDT expired. */
#define ERR_PNS_IF_CM_AR_REASON_MISS     0xC0300A05L

/** AR cmi timeout. */
#define ERR_PNS_IF_CM_AR_REASON_TIMER    0xC0300A06L

/** AR alarm-open failed. */
#define ERR_PNS_IF_CM_AR_REASON_ALARM    0xC0300A07L

/** AR alarm-send.cnf(-). */
#define ERR_PNS_IF_CM_AR_REASON_ALSND    0xC0300A08L

/** AR alarm-ack-send.cnf(-). */
#define ERR_PNS_IF_CM_AR_REASON_ALACK    0xC0300A09L

/** AR alarm data too long. */
#define ERR_PNS_IF_CM_AR_REASON_ALLEN    0xC0300A0AL

/** AR alarm.ind(err). */
#define ERR_PNS_IF_CM_AR_REASON_ASRT     0xC0300A0BL

/** AR rpc-client call.cnf(-). */
#define ERR_PNS_IF_CM_AR_REASON_RPC      0xC0300A0CL

/** AR abort.req. */
#define ERR_PNS_IF_CM_AR_REASON_ABORT    0xC0300A0DL

/** AR re-run aborts existing AR. */
#define ERR_PNS_IF_CM_AR_REASON_RERUN    0xC0300A0EL

/** AR release.ind received. */
#define ERR_PNS_IF_CM_AR_REASON_REL      0xC0300A0FL

/** AR device deactivated. */
#define ERR_PNS_IF_CM_AR_REASON_PAS      0xC0300A10L

/** AR removed. */
#define ERR_PNS_IF_CM_AR_REASON_RMV      0xC0300A11L

/** AR protocol violation. */
#define ERR_PNS_IF_CM_AR_REASON_PROT     0xC0300A12L

/** AR name resolution error. */
#define ERR_PNS_IF_CM_AR_REASON_NARE     0xC0300A13L

/** AR RPC-Bind error. */
#define ERR_PNS_IF_CM_AR_REASON_BIND     0xC0300A14L

/** AR RPC-Connect error. */
#define ERR_PNS_IF_CM_AR_REASON_CONNECT  0xC0300A15L

/** AR RPC-Read error. */
#define ERR_PNS_IF_CM_AR_REASON_READ     0xC0300A16L

/** AR RPC-Write error. */
#define ERR_PNS_IF_CM_AR_REASON_WRITE    0xC0300A17L

/** AR RPC-Control error. */
#define ERR_PNS_IF_CM_AR_REASON_CONTROL  0xC0300A18L

/** AR forbidden pull or plug after check.rsp and before in-data.ind. */
#define ERR_PNS_IF_CM_AR_REASON_PULLPLUG 0xC0300A19L

/** AR AP removed. */
#define ERR_PNS_IF_CM_AR_REASON_AP_RMV   0xC0300A1AL

/** AR link down. */
#define ERR_PNS_IF_CM_AR_REASON_LNK_DWN  0xC0300A1BL

/** AR could not register multicast-mac address. */
#define ERR_PNS_IF_CM_AR_REASON_MMAC     0xC0300A1CL

/** Not synchronized (cannot start companion-ar). */
#define ERR_PNS_IF_CM_AR_REASON_SYNC     0xC0300A1DL

/** Wrong topology (cannot start companion-ar). */
#define ERR_PNS_IF_CM_AR_REASON_TOPO     0xC0300A1EL

/** DCP, station-name changed. */
#define ERR_PNS_IF_CM_AR_REASON_DCP_NAME 0xC0300A1FL

/** DCP, reset to factory-settings. */
#define ERR_PNS_IF_CM_AR_REASON_DCP_RESET 0xC0300A20L

/** 0x8ipp submodule in the first AR has either an appl-ready-pending (erroneous parameterization) or is locked (no parameterization) or is wrong or pulled (no parameterization). */
#define ERR_PNS_IF_CM_AR_REASON_PRM      0xC0300A21L

/** No irdata record yet. */
#define ERR_PNS_IF_CM_AR_REASON_IRDATA   0xC0300A22L

/** Ownership of PDEV. */
#define ERR_PNS_IF_CM_AR_REASON_PDEV     0xC0300A23L

/** Max. Unused. */
#define ERR_PNS_IF_CM_AR_REASON_MAX      0xC0300AFFL

/* END: On 12.11.08 Konstantin added: Area for Abort- and Offline- Reason error codes to support Siemens Stack */
/* Message IDs 0x0A00 - 0x0AFF */
#endif  /* PNS_RESULTS_H_ */
