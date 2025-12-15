/*
 * pns_functions.c
 *
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#include "pns_functions.h"

#include "app_config.h"
#include "SystemPackets.h"
#include "cifXUser.h"
#include "cifXErrors.h"

#define PNS_SUBMODULE_COUNT    6 //With 18, the configuration packet is to bit an will be send fragmented

/**************************************************************************************
 function:    BuildConfigParams
 description: Fills all necessary device parameters in set configuration packet.

 global:      none
 input:       PNS_IF_DEVICE_PARAMETER_T FAR* ptSetConfig    - pointer to the packet
 UINT16 usDeviceClass                          - netX device class

 output:      none
 return:      long ( size of pointer to the packet )
 **************************************************************************************/
static long BuildConfigParams(void* pvPck, uint16_t usDeviceClass)
{
  PNS_IF_DEVICE_PARAMETER_T *ptSetConfig = pvPck;

  /* First set arrays to zero */
  memset(ptSetConfig->abNameOfStation, 0x00, 240); /* The NameOfStation as ASCII char-array  */
  memset(ptSetConfig->abTypeOfStation, 0x00, 240); /* The TypeOfStation as ASCII char-array  */
  memset(ptSetConfig->abDeviceType, 0x00, 28); /* The DeviceType as ASCII char-array     */
  memset(ptSetConfig->abOrderId, 0x00, 20); /* The OrderId as ASCII char-array        */

  ptSetConfig->ulSystemFlags = PNS_IF_SYSTEM_START_AUTO_START |
                              PNS_IF_SYSTEM_STACK_HANDLE_I_M_ENABLED |
                              PNS_IF_SYSTEM_ARDY_WOUT_APPL_REG_ENABLED |
                              PNS_IF_SYSTEM_NAME_IP_HANDLING_BY_STACK_ENABLED;

  ptSetConfig->ulWdgTime = 1000; /* Watchdog time (in milliseconds)        */
  ptSetConfig->ulVendorId = 0x011E; /* Vendor ID                              */
  ptSetConfig->ulMaxAr = 0; /* Currently not used                     */
  ptSetConfig->ulCompleteInputSize = 0; /* Maximum amount of allowed input data   */
  ptSetConfig->ulCompleteOutputSize = 0; /* Maximum amount of allowed output data  */

  ptSetConfig->ulDeviceId = 0x1003; /* Device ID                              */

  PRINTF("DeviceID: %u\r\n", (unsigned int) ptSetConfig->ulDeviceId);

  memcpy(ptSetConfig->abNameOfStation, "stxv5repns", sizeof("stxv5repns")); /* The NameOfStation as ASCII char-array  */
  ptSetConfig->ulNameOfStationLen = sizeof("stxV5repns") - 1; /* Length of NameOfStation                */
  memcpy(ptSetConfig->abTypeOfStation, "STX V5 RE/PNS", sizeof("STX V5 RE/PNS")); /* The TypeOfStation as ASCII char-array  */

  ptSetConfig->ulTypeOfStationLen = sizeof("STX V5 RE/PNS") - 1; /* Length of TypeOfStation                */
  memcpy(ptSetConfig->abDeviceType, "STX RE/PNS", sizeof("STX RE/PNS") - 1); /* The DeviceType as ASCII char-array     */

  /*Set Device Name and IP with configuration packet works only if Flag:
   PNS_IF_SYSTEM_NAME_IP_HANDLING_BY_STACK_ENABLED is disabled. */
  ptSetConfig->ulIpAddr =  0;//0; /* IP address, default: 0.0.0.0                         */
  ptSetConfig->ulNetMask = 0; /* Network mask, default: 0.0.0.0                       */
  ptSetConfig->ulGateway = 0; /* Gateway address, default: 0.0.0.0                    */
  ptSetConfig->usSwRevision1 = 5; /* Software Revision 1, default: 0                      */
  ptSetConfig->usSwRevision2      = 7;                                                   /* Software Revision 2, default: 0                      */
  ptSetConfig->usSwRevision3 = 0; /* Software Revision 3, default: 0                      */
  ptSetConfig->bSwRevisionPrefix = 'V'; /* Software Revision Prefix, default: 0                 */
  ptSetConfig->bReserved = 0; /* Reserved, set to zero                                */
  ptSetConfig->usMaxDiagRecords = 16; /* The number of diagnosis records, default: 256        */
  ptSetConfig->usInstanceId = 1; /* Instance ID, GSDML ObjectUUID_LocalIndex, default: 1 */
  ptSetConfig->usReserved = 0; /* Reserved for future use, set to zero                 */

  return sizeof(*ptSetConfig);
} /* BuildConfigParams */

/**************************************************************************************
 function:    BuildModulConfig
 description: Fills all necessary information about module configuration.

 global:      none
 input:       PNS_IF_MODULE_CFG_REQ_DATA_T FAR* ptModule    - pointer to the packet

 output:      none
 return:      long (Pointer to the packet)
 **************************************************************************************/
static long BuildModulConfig(void* pvPck)
{
  PNS_IF_MODULE_CFG_REQ_DATA_T *ptModule = pvPck;

  PNS_IF_API_STRUCT_T *ptApi = NULL;
  PNS_IF_SUBMODULE_STRUCT_T *ptSubmod = NULL;

  ptModule->ulNumApi = 0x01; /* Number of API elements to follow */
  ptApi = (PNS_IF_API_STRUCT_T *) (ptModule + 1); /* Pointer to Module configuration  */

  ptApi->ulApi = 0x00; /* Number of the API profile to be configured   */
  if(ptApi->ulNumSubmodItems != PNS_SUBMODULE_COUNT) /* Number of submodule-items this API contains  */
  {
    while(1);
  }

  ptSubmod = (PNS_IF_SUBMODULE_STRUCT_T *) (ptApi + 1); /* Pointer to Submodule configuration           */

  /* Configure the DAP V5.7 */
  ptSubmod->usSlot = 0x0000; /* Slot this submodule belongs to                                         */
  ptSubmod->usSubslot = 0x0001; /* Subslot of this submodule                                              */
  ptSubmod->ulModuleId = 0x00003021; /* Module ID to which this submodule belongs                              */
  ptSubmod->ulSubmodId = 0x00003020; /* Submodule ID                                                           */
  ptSubmod->ulProvDataLen = 0x00000000; /* Length of provider data (sent by IO-Device, received by IO-Controller) */
  ptSubmod->ulConsDataLen = 0x00000000; /* Length of consumer data (sent by IO-Controller, received by IO-Device) */
  ptSubmod->ulDPMOffsetOut = 0x00000000; /* Offset in DPM OutputArea or in Output-image, where provided data of the*/
  /* submodule shall be taken from                                          */
  ptSubmod->ulDPMOffsetIn = 0x00000000; /* Offset in DPM InputArea or in Input-image, where consumed data for the */
  /* submodule shall be copied to                                           */
  ptSubmod->usOffsetIOPSProvider = 0x0000; /* Offset where the stack shall take the IOPS provider state for this     */
  /* submodule relative to beginning of IOPS block in DPM output area from  */
  ptSubmod->usOffsetIOPSConsumer = 0x0000; /* Offset where the stack shall put the IOPS consumer state of this       */
  /* submodule relative to beginning of IOPS block in DPM input area to     */
  ptSubmod->usOffsetIOCSProvider = 0x0000; /* Offset where the stack shall take the IOCS provider state for this     */
  /* submodule relative to beginning of IOCS block in DPM output area from  */
  ptSubmod->usOffsetIOCSConsumer = 0x0000; /* Offset where the stack shall put the IOCS consumer state for this      */
  /* submodule relative to beginning of IOCS block in DPM input area to     */
  ptSubmod->ulReserved = 0x00000000; /* Reserved for future usage                                              */
  ptSubmod++; /* Increment submodule                                                    */

  /* Configure PDEV PN-IO */
  ptSubmod->usSlot = 0x0000; /* Slot this submodule belongs to                                         */
  ptSubmod->usSubslot = 0x8000; /* Subslot of this submodule                                              */
  ptSubmod->ulModuleId = 0x00003021; /* Module ID to which this submodule belongs                              */
  ptSubmod->ulSubmodId = 0x00003021; /* Submodule ID                                                           */
  ptSubmod->ulProvDataLen = 0x00000000; /* Length of provider data (sent by IO-Device, received by IO-Controller) */
  ptSubmod->ulConsDataLen = 0x00000000; /* Length of consumer data (sent by IO-Controller, received by IO-Device) */
  ptSubmod->ulDPMOffsetOut = 0x00000000; /* Offset in DPM OutputArea or in Output-image, where provided data of the*/
  /* submodule shall be taken from                                          */
  ptSubmod->ulDPMOffsetIn = 0x00000000; /* Offset in DPM InputArea or in Input-image, where consumed data for the */
  /* submodule shall be copied to                                           */
  ptSubmod->usOffsetIOPSProvider = 0x0000; /* Offset where the stack shall take the IOPS provider state for this     */
  /* submodule relative to beginning of IOPS block in DPM output area from  */
  ptSubmod->usOffsetIOPSConsumer = 0x0000; /* Offset where the stack shall put the IOPS consumer state of this       */
  /* submodule relative to beginning of IOPS block in DPM input area to     */
  ptSubmod->usOffsetIOCSProvider = 0x0000; /* Offset where the stack shall take the IOCS provider state for this     */
  /* submodule relative to beginning of IOCS block in DPM output area from  */
  ptSubmod->usOffsetIOCSConsumer = 0x0000; /* Offset where the stack shall put the IOCS consumer state for this      */
  /* submodule relative to beginning of IOCS block in DPM input area to     */
  ptSubmod->ulReserved = 0x00000000; /* Reserved for future usage                                              */
  ptSubmod++; /* Increment submodule                                                    */

  /* Configure PDEV Port 1 */
  ptSubmod->usSlot = 0x0000; /* Slot this submodule belongs to                                         */
  ptSubmod->usSubslot = 0x8001; /* Subslot of this submodule                                              */
  ptSubmod->ulModuleId = 0x00003021; /* Module ID to which this submodule belongs                              */
  ptSubmod->ulSubmodId = 0x00003022; /* Submodule ID                                                           */
  ptSubmod->ulProvDataLen = 0x00000000; /* Length of provider data (sent by IO-Device, received by IO-Controller) */
  ptSubmod->ulConsDataLen = 0x00000000; /* Length of consumer data (sent by IO-Controller, received by IO-Device) */
  ptSubmod->ulDPMOffsetOut = 0x00000000; /* Offset in DPM OutputArea or in Output-image, where provided data of the*/
  /* submodule shall be taken from                                          */
  ptSubmod->ulDPMOffsetIn = 0x00000000; /* Offset in DPM InputArea or in Input-image, where consumed data for the */
  /* submodule shall be copied to                                           */
  ptSubmod->usOffsetIOPSProvider = 0x0000; /* Offset where the stack shall take the IOPS provider state for this     */
  /* submodule relative to beginning of IOPS block in DPM output area from  */
  ptSubmod->usOffsetIOPSConsumer = 0x0000; /* Offset where the stack shall put the IOPS consumer state of this       */
  /* submodule relative to beginning of IOPS block in DPM input area to     */
  ptSubmod->usOffsetIOCSProvider = 0x0000; /* Offset where the stack shall take the IOCS provider state for this     */
  /* submodule relative to beginning of IOCS block in DPM output area from  */
  ptSubmod->usOffsetIOCSConsumer = 0x0000; /* Offset where the stack shall put the IOCS consumer state for this      */
  /* submodule relative to beginning of IOCS block in DPM input area to     */
  ptSubmod->ulReserved = 0x00000000; /* Reserved for future usage                                              */
  ptSubmod++; /* Increment submodule                                                    */

  /* Configure PDEV Port 2 */
  ptSubmod->usSlot = 0x0000; /* Slot this submodule belongs to                                         */
  ptSubmod->usSubslot = 0x8002; /* Subslot of this submodule                                              */
  ptSubmod->ulModuleId = 0x00003021; /* Module ID to which this submodule belongs                              */
  ptSubmod->ulSubmodId = 0x00003023; /* Submodule ID                                                           */
  ptSubmod->ulProvDataLen = 0x00000000; /* Length of provider data (sent by IO-Device, received by IO-Controller) */
  ptSubmod->ulConsDataLen = 0x00000000; /* Length of consumer data (sent by IO-Controller, received by IO-Device) */
  ptSubmod->ulDPMOffsetOut = 0x00000000; /* Offset in DPM OutputArea or in Output-image, where provided data of the*/
  /* submodule shall be taken from                                          */
  ptSubmod->ulDPMOffsetIn = 0x00000000; /* Offset in DPM InputArea or in Input-image, where consumed data for the */
  /* submodule shall be copied to                                           */
  ptSubmod->usOffsetIOPSProvider = 0x0000; /* Offset where the stack shall take the IOPS provider state for this     */
  /* submodule relative to beginning of IOPS block in DPM output area from  */
  ptSubmod->usOffsetIOPSConsumer = 0x0000; /* Offset where the stack shall put the IOPS consumer state of this       */
  /* submodule relative to beginning of IOPS block in DPM input area to     */
  ptSubmod->usOffsetIOCSProvider = 0x0000; /* Offset where the stack shall take the IOCS provider state for this     */
  /* submodule relative to beginning of IOCS block in DPM output area from  */
  ptSubmod->usOffsetIOCSConsumer = 0x0000; /* Offset where the stack shall put the IOCS consumer state for this      */
  /* submodule relative to beginning of IOCS block in DPM input area to     */
  ptSubmod->ulReserved = 0x00000000; /* Reserved for future usage                                              */
  ptSubmod++;

  /* Configure 10 Byte Input module (Device to PLC) */
  ptSubmod->usSlot = 0x0001; /* Slot this submodule belongs to                                         */
  ptSubmod->usSubslot = 0x0001; /* Subslot of this submodule                                              */
  /* PNS SET CONFIG Dirk */
  ptSubmod->ulModuleId = 0x00000001; /* Module ID to which this submodule belongs                              */
  ptSubmod->ulSubmodId = 0x00000001; /* Submodule ID                                                           */
  ptSubmod->ulProvDataLen = 0x0000000A; /* Length of provider data (sent by IO-Device, received by IO-Controller) */
  ptSubmod->ulConsDataLen = 0x00000000; /* Length of consumer data (sent by IO-Controller, received by IO-Device) */
  ptSubmod->ulDPMOffsetOut = 0x00000000; /* Offset in DPM OutputArea or in Output-image, where provided data of the*/
  /* submodule shall be taken from                                          */
  ptSubmod->ulDPMOffsetIn = 0x00000000; /* Offset in DPM InputArea or in Input-image, where consumed data for the */
  /* submodule shall be copied to                                           */
  ptSubmod->usOffsetIOPSProvider = 0x0000; /* Offset where the stack shall take the IOPS provider state for this     */
  /* submodule relative to beginning of IOPS block in DPM output area from  */
  ptSubmod->usOffsetIOPSConsumer = 0x0000; /* Offset where the stack shall put the IOPS consumer state of this       */
  /* submodule relative to beginning of IOPS block in DPM input area to     */
  ptSubmod->usOffsetIOCSProvider = 0x0000; /* Offset where the stack shall take the IOCS provider state for this     */
  /* submodule relative to beginning of IOCS block in DPM output area from  */
  ptSubmod->usOffsetIOCSConsumer = 0x0000; /* Offset where the stack shall put the IOCS consumer state for this      */
  /* submodule relative to beginning of IOCS block in DPM input area to     */
  ptSubmod->ulReserved = 0x00000000; /* Reserved for future usage                                              */
  ptSubmod++; /* Increment submodule                                                    */

  /* Configure 6 Byte Output module (PLC to Device) */
  ptSubmod->usSlot = 0x0002; /* Slot this submodule belongs to                                         */
  ptSubmod->usSubslot = 0x0001; /* Subslot of this submodule                                              */
  /* PNS SET CONFIG Dirk */
  ptSubmod->ulModuleId = 0x00000002; /* Module ID to which this submodule belongs                              */
  ptSubmod->ulSubmodId = 0x00000001; /* Submodule ID                                                           */
  ptSubmod->ulProvDataLen = 0x00000000; /* Length of provider data (sent by IO-Device, received by IO-Controller) */
  ptSubmod->ulConsDataLen = 0x00000006; /* Length of consumer data (sent by IO-Controller, received by IO-Device) */
  ptSubmod->ulDPMOffsetOut = 0x00000000; /* Offset in DPM OutputArea or in Output-image, where provided data of the*/
  /* submodule shall be taken from                                          */
  ptSubmod->ulDPMOffsetIn = 0x00000000; /* Offset in DPM InputArea or in Input-image, where consumed data for the */
  /* submodule shall be copied to                                           */
  ptSubmod->usOffsetIOPSProvider = 0x0000; /* Offset where the stack shall take the IOPS provider state for this     */
  /* submodule relative to beginning of IOPS block in DPM output area from  */
  ptSubmod->usOffsetIOPSConsumer = 0x0000; /* Offset where the stack shall put the IOPS consumer state of this       */
  /* submodule relative to beginning of IOPS block in DPM input area to     */
  ptSubmod->usOffsetIOCSProvider = 0x0000; /* Offset where the stack shall take the IOCS provider state for this     */
  /* submodule relative to beginning of IOCS block in DPM output area from  */
  ptSubmod->usOffsetIOCSConsumer = 0x0000; /* Offset where the stack shall put the IOCS consumer state for this      */
  /* submodule relative to beginning of IOCS block in DPM input area to     */
  ptSubmod->ulReserved = 0x00000000; /* Reserved for future usage                                              */
  ptSubmod++; /* Increment submodule                                                    */

  for(int i = 0; i < PNS_SUBMODULE_COUNT - 6u; i++)
  {
    /* Configure 6 Byte Output module (PLC to Device) */
    ptSubmod->usSlot = 0x0003 + i; /* Slot this submodule belongs to                                         */
    ptSubmod->usSubslot = 0x0001; /* Subslot of this submodule                                              */
    /* PNS SET CONFIG Dirk */
    ptSubmod->ulModuleId = 0x00000002; /* Module ID to which this submodule belongs                              */
    ptSubmod->ulSubmodId = 0x00000001; /* Submodule ID                                                           */
    ptSubmod->ulProvDataLen = 0x00000000; /* Length of provider data (sent by IO-Device, received by IO-Controller) */
    ptSubmod->ulConsDataLen = 0x00000006; /* Length of consumer data (sent by IO-Controller, received by IO-Device) */
    ptSubmod->ulDPMOffsetOut = 0x00000000; /* Offset in DPM OutputArea or in Output-image, where provided data of the*/
    /* submodule shall be taken from                                          */
    ptSubmod->ulDPMOffsetIn = 0x00000006 + 6u * i; /* Offset in DPM InputArea or in Input-image, where consumed data for the */
    /* submodule shall be copied to                                           */
    ptSubmod->usOffsetIOPSProvider = 0x0000; /* Offset where the stack shall take the IOPS provider state for this     */
    /* submodule relative to beginning of IOPS block in DPM output area from  */
    ptSubmod->usOffsetIOPSConsumer = 0x0000; /* Offset where the stack shall put the IOPS consumer state of this       */
    /* submodule relative to beginning of IOPS block in DPM input area to     */
    ptSubmod->usOffsetIOCSProvider = 0x0000; /* Offset where the stack shall take the IOCS provider state for this     */
    /* submodule relative to beginning of IOCS block in DPM output area from  */
    ptSubmod->usOffsetIOCSConsumer = 0x0000; /* Offset where the stack shall put the IOCS consumer state for this      */
    /* submodule relative to beginning of IOCS block in DPM input area to     */
    ptSubmod->ulReserved = 0x00000000; /* Reserved for future usage                                              */
    ptSubmod++; /* Increment submodule                                                    */
  }
  return (long) ptSubmod - (long) ptModule;
} /* BuildModulConfig */

uint32_t  PNS_ConfigureStack(PNS_RESSOURCES_T* ptRsc)
{
  uint32_t ulRet = CIFX_NO_ERROR;
  CIFX_PACKET *ptPacket = &ptRsc->tPacket;

  uint16_t usDeviceClass = 0;

  struct
  {
    /** Data part */
    PNS_IF_SET_CONFIGURATION_REQUEST_DATA_T tSetConfigReq;
    /** Submodule part */
    PNS_IF_API_STRUCT_T tAPI;
    /** Submodule part */
    PNS_IF_SUBMODULE_STRUCT_T tSubmodules[PNS_SUBMODULE_COUNT];
  } tConfiguration;

  /* Clear packet */
  memset(ptPacket, 0, sizeof(*ptPacket));

  tConfiguration.tSetConfigReq = (PNS_IF_SET_CONFIGURATION_REQUEST_DATA_T ) { 0 };
  tConfiguration.tSetConfigReq.tModuleConfig.ulNumApi = 0x01u;
  tConfiguration.tAPI.ulNumSubmodItems = PNS_SUBMODULE_COUNT;

  tConfiguration.tSetConfigReq.ulTotalConfigPckLen = sizeof(tConfiguration);

  ptPacket->tHeader.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  ptPacket->tHeader.ulId = 0x1u;
  ptPacket->tHeader.ulCmd = PNS_IF_SET_CONFIGURATION_REQ;
  (void) BuildConfigParams(&tConfiguration.tSetConfigReq.tDeviceParameters, usDeviceClass);
  (void) BuildModulConfig(&tConfiguration.tSetConfigReq.tModuleConfig);

  if(sizeof(tConfiguration) < sizeof(ptPacket->abData))
  {
    // No Fragmentation
    ptPacket->tHeader.ulLen = sizeof(tConfiguration);
    memcpy(ptPacket->abData, &tConfiguration, sizeof(tConfiguration));
  }
  else
  {
    // Fragmentation
    // First Packet
    size_t ulIndex = 0;
    size_t ulOffset = 0;
    ptPacket->tHeader.ulLen = sizeof(ptPacket->abData);
    ptPacket->tHeader.ulCmd = PNS_IF_SET_CONFIGURATION_REQ;
    ptPacket->tHeader.ulExt &= ~(HIL_PACKET_SEQ_MASK | HIL_PACKET_SEQ_NR_MASK);
    ptPacket->tHeader.ulExt |= HIL_PACKET_SEQ_FIRST | ulIndex;
    memcpy(ptPacket->abData, &((uint8_t*) &tConfiguration)[ulOffset], ptPacket->tHeader.ulLen);

    ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);

    if(CIFX_NO_ERROR != ulRet)
      return ulRet;

    if(SUCCESS_HIL_OK != ptRsc->tPacket.tHeader.ulState)
      return ptRsc->tPacket.tHeader.ulState;

    ulIndex++;
    ulOffset = sizeof(ptPacket->abData) * ulIndex;
    while(ulOffset < (sizeof(tConfiguration) - sizeof(ptPacket->abData)))
    {
      // Middle Packets
      ptPacket->tHeader.ulLen = sizeof(ptPacket->abData);
      ptPacket->tHeader.ulCmd = PNS_IF_SET_CONFIGURATION_REQ;
      ptPacket->tHeader.ulExt &= ~(HIL_PACKET_SEQ_MASK | HIL_PACKET_SEQ_NR_MASK);
      ptPacket->tHeader.ulExt |= HIL_PACKET_SEQ_MIDDLE | ulIndex;
      memcpy(ptPacket->abData, &((uint8_t*) &tConfiguration)[ulOffset], ptPacket->tHeader.ulLen);

      ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);
      if(CIFX_NO_ERROR != ulRet)
        return ulRet;

      if(SUCCESS_HIL_OK != ptRsc->tPacket.tHeader.ulState)
        return ptRsc->tPacket.tHeader.ulState;

      ulIndex++;
      ulOffset = sizeof(ptPacket->abData) * ulIndex;
    }
    // Final Packet
    ptPacket->tHeader.ulLen = sizeof(tConfiguration) - ulOffset;
    ptPacket->tHeader.ulCmd = PNS_IF_SET_CONFIGURATION_REQ;
    ptPacket->tHeader.ulExt &= ~(HIL_PACKET_SEQ_MASK | HIL_PACKET_SEQ_NR_MASK);
    ptPacket->tHeader.ulExt |= HIL_PACKET_SEQ_LAST | ulIndex;
    memcpy(ptPacket->abData, &((uint8_t*) &tConfiguration)[ulOffset], sizeof(tConfiguration) - ulOffset);
  }

  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);
  if(CIFX_NO_ERROR != ulRet)
    return ulRet;

  if(SUCCESS_HIL_OK != ptRsc->tPacket.tHeader.ulState)
    return ptRsc->tPacket.tHeader.ulState;

  SysPkt_AssembleChannelInitReq(ptPacket);
  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);
  if(CIFX_NO_ERROR != ulRet)
    return ulRet;

  if(SUCCESS_HIL_OK != ptRsc->tPacket.tHeader.ulState)
    return ptRsc->tPacket.tHeader.ulState;

  SysPkt_AssembleStartStopCommReq(ptPacket, true);
  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);
  if(CIFX_NO_ERROR != ulRet)
    return ulRet;

  if(SUCCESS_HIL_OK != ptRsc->tPacket.tHeader.ulState)
    return ptRsc->tPacket.tHeader.ulState;


  ptRsc->fDeviceIsRunning = true;
  return CIFX_NO_ERROR;
}

#ifdef HOST_APPLICATION_SETS_SERIAL_NUMBER
uint32_t PNS_SetOemData(PNS_RESSOURCES_T* ptRsc)
{
  uint32_t ulRet = CIFX_NO_ERROR;
  HIL_DDP_SERVICE_SET_REQ_T *ptReq = (HIL_DDP_SERVICE_SET_REQ_T*) &ptRsc->tPacket;
  char *szSerialNumber = "20000           ";
  char *szOrderNumber = "1251.100            ";
  char *szHardwareRevision = "6";

  ptReq->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  ptReq->tHead.ulCmd = HIL_DDP_SERVICE_SET_REQ;
  ptReq->tHead.ulLen = sizeof(ptReq->tData.ulDataType) + strlen(szSerialNumber) + 1;
  ptReq->tData.ulDataType = HIL_DDP_SERVICE_DATATYPE_OEM_SERIALNUMBER;

  memcpy(ptReq->tData.uDataType.szString, szSerialNumber, strlen(szSerialNumber) + 1);

  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);

  /* Check status of cifX function and confirmation packet */
  ulRet = (ulRet != CIFX_NO_ERROR) ? ulRet : ptRsc->tPacket.tHeader.ulState;
  if(CIFX_NO_ERROR != ulRet)
    return ulRet;

  ptReq->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  ptReq->tHead.ulCmd = HIL_DDP_SERVICE_SET_REQ;
  ptReq->tHead.ulLen = sizeof(ptReq->tData.ulDataType) + strlen(szOrderNumber) + 1;
  ptReq->tData.ulDataType = HIL_DDP_SERVICE_DATATYPE_OEM_ORDERNUMBER;

  memcpy(ptReq->tData.uDataType.szString, szOrderNumber, strlen(szOrderNumber) + 1);

  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);

  /* Check status of cifX function and confirmation packet */
  ulRet = (ulRet != CIFX_NO_ERROR) ? ulRet : ptRsc->tPacket.tHeader.ulState;
  if(CIFX_NO_ERROR != ulRet)
    return ulRet;

  ptReq->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  ptReq->tHead.ulCmd = HIL_DDP_SERVICE_SET_REQ;
  ptReq->tHead.ulLen = sizeof(ptReq->tData.ulDataType) + strlen(szHardwareRevision) + 1;
  ptReq->tData.ulDataType = HIL_DDP_SERVICE_DATATYPE_OEM_HARDWAREREVISION;

  memcpy(ptReq->tData.uDataType.szString, szHardwareRevision, strlen(szHardwareRevision) + 1);

  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);

  /* Check status of cifX function and confirmation packet */
  ulRet = (CIFX_NO_ERROR != ulRet) ? ulRet : ptRsc->tPacket.tHeader.ulState;
  if(CIFX_NO_ERROR != ulRet)
    return ulRet;

  /* also render the OEM serial number "valid" in the corresponding bitfield */
  ptReq->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  ptReq->tHead.ulCmd = HIL_DDP_SERVICE_SET_REQ;
  ptReq->tHead.ulLen = sizeof(ptReq->tData.ulDataType) + sizeof(ptReq->tData.uDataType.ulValue);
  ptReq->tData.ulDataType = HIL_DDP_SERVICE_DATATYPE_OEM_OPTIONS;
  ptReq->tData.uDataType.ulValue = HIL_PRODUCT_DATA_OEM_IDENTIFICATION_FLAG_ORDERNUMBER_VALID
                                  | HIL_PRODUCT_DATA_OEM_IDENTIFICATION_FLAG_HARDWAREREVISION_VALID
                                  | HIL_PRODUCT_DATA_OEM_IDENTIFICATION_FLAG_SERIALNUMBER_VALID
                                  | HIL_PRODUCT_DATA_OEM_IDENTIFICATION_FLAG_PRODUCTIONDATA_VALID; /* set bit zero to set OEM serial number valid */

  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);

  /* Check status of cifX function and confirmation packet */
  return (CIFX_NO_ERROR != ulRet) ? ulRet : ptRsc->tPacket.tHeader.ulState;
}
#endif

#if defined(HOST_APPLICATION_SETS_MAC_ADDRESS) || defined(HOST_APPLICATION_SETS_SERIAL_NUMBER)
uint32_t  PNS_ActivateDdp(PNS_RESSOURCES_T* ptRsc)
{
  uint32_t ulRet = CIFX_NO_ERROR;
  HIL_DDP_SERVICE_SET_REQ_T *ptReq = (HIL_DDP_SERVICE_SET_REQ_T*) &ptRsc->tPacket;

  /* required when inital DPP state is passive: Set DDP active now */
  ptReq->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  ptReq->tHead.ulCmd = HIL_DDP_SERVICE_SET_REQ;
  ptReq->tHead.ulLen = sizeof(ptReq->tData.ulDataType) + sizeof(ptReq->tData.uDataType.ulValue);
  ptReq->tData.ulDataType = HIL_DDP_SERVICE_DATATYPE_STATE;
  ptReq->tData.uDataType.ulValue = HIL_DDP_SERVICE_STATE_ACTIVE;

  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);

  /* Check status of cifX function and confirmation packet */
  return (CIFX_NO_ERROR != ulRet) ? ulRet : ptRsc->tPacket.tHeader.ulState;
}
#endif

#ifdef HOST_APPLICATION_SETS_MAC_ADDRESS
uint32_t  PNS_SetMacAddress(PNS_RESSOURCES_T* ptRsc)
{
  uint32_t                   ulRet = CIFX_NO_ERROR;
  HIL_DDP_SERVICE_SET_REQ_T* ptReq = (HIL_DDP_SERVICE_SET_REQ_T*)&ptRsc->tPacket;

  uint8_t abMyComMacAddresses[8][6] =
  {
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x0 },   /* This is the first chassis MAC address which is used by EtherNet/IP */
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x1 },   /* Port 0 MAC Address used for LLDP                                   */
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x2 },   /* Port 1 MAC Address used for LLDP                                   */
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x3 },   /* This is the second chassis MAC (RawEthernet/NDIS)                  */
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x4 },
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x5 },
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x6 },
    { 0xa, 0xb, 0xc, 0xd, 0xe, 0x7 },
  };

  ptReq->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
  ptReq->tHead.ulCmd = HIL_DDP_SERVICE_SET_REQ;
  ptReq->tHead.ulLen = sizeof(ptReq->tData.ulDataType) + sizeof(abMyComMacAddresses);

  /* Set MAC adrress for the protocol stack (override pre-defined MAC address from FDL) */
  ptReq->tData.ulDataType = HIL_DDP_SERVICE_DATATYPE_MAC_ADDRESSES_COM;

  memcpy(ptReq->tData.uDataType.atMacAddress, abMyComMacAddresses, sizeof(abMyComMacAddresses));

  ulRet = Pkt_SendReceivePacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TXRX_TIMEOUT);
  return (CIFX_NO_ERROR != ulRet) ? ulRet : ptRsc->tPacket.tHeader.ulState;
}
#endif

bool PNS_PacketHandler(CIFX_PACKET* ptPacket, void* pvUserData)
{
  PNS_RESSOURCES_T *ptRsc = (PNS_RESSOURCES_T*)pvUserData;
  bool fPacketCouldBeHandled = false;

  if(ptPacket != &(ptRsc->tPacket))
  {
    PRINTF("Unexpected packet resource received!!!" NEWLINE);
    return false;
  }

  switch (ptPacket->tHeader.ulCmd)
  {
  default:
    if((ptRsc->tPacket.tHeader.ulCmd & 0x1) == 0) /* received an indication*/
    {
      PRINTF("Warning: Disregarded indication packet received!" NEWLINE);
      ptRsc->tPacket.tHeader.ulLen = 0;
      ptRsc->tPacket.tHeader.ulState = SUCCESS_HIL_OK;
      ptRsc->tPacket.tHeader.ulCmd |= 0x01; /* Make it a response */

      (void) Pkt_SendPacket(ptRsc->hPktIfRsc, &(ptRsc->tPacket), TX_TIMEOUT);
      fPacketCouldBeHandled = true;
    }
    else
    { /* received a confirmation */
    PRINTF("Warning: Disregarded confirmation packet received!" NEWLINE);
  }
  break;
  } /*switch*/

  return fPacketCouldBeHandled;
}
