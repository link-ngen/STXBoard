/**************************************************************************************
Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.
***************************************************************************************
$Id:  $:

Description:
 Profinet Core Public Packet IOCTL
**************************************************************************************/
#ifndef _PN_PUBLIC_IOCTL_H_
#define _PN_PUBLIC_IOCTL_H_

#include <stdint.h>
#include "Hil_Packet.h"
#include "Hil_Types.h"

#ifndef ALIGNED_TYPEDEF
#define PN_ALIGNED_TYPEDEF(type, align) typedef type  ALIGN ## align ## _ ## type __attribute__((aligned(align)))

#define ALIGNED_TYPEDEF(type, align) PN_ALIGNED_TYPEDEF(type,align)
#endif

/** \def ARRAYS_OF_LENGTH_ZERO

  The macro ARRAYS_OF_LENGTH_ZERO can be used for array with variable-length object. Like:
  \code
  struct line {
    int length;
    char contents[0];
  };
  \endcode

  \note If the used Compiler is not be able to use ARRAYS_OF_LENGTH_ZERO with value 0 you have to define ARRAYS_OF_LENGTH_ZERO with e.g. 1
*/
#ifndef ARRAYS_OF_LENGTH_ZERO
  #define ARRAYS_OF_LENGTH_ZERO
#endif

/**
 * Profinet Core Stack IOCTL packet interface
 * @ingroup pn_core_ioctl
 *
 *  Command codes for the Requests supported/used Profinet Core the request/confirmation
 *  pair is used by the host application to make the  stack to do something.
**/

typedef enum PN_CORE_IOCTL_CMD_Etag
{
  /** HIL_PACKET_HEADER_T.ulCmd : code for the Packet-based services  */
  PN_CORE_CMD_IOCTL_REQ                     = 0x00008620,
  PN_CORE_CMD_IOCTL_CNF                     = 0x00008621,

  PN_CORE_CMD_GET_NEIGHBORHOOD_INFO_REQ     = 0x00008622,
  PN_CORE_CMD_GET_NEIGHBORHOOD_INFO_CNF     = 0x00008623,

} PN_CORE_IOCTL_CMD_E;


/*
 *  IOCTL input/output control code for PN_CORE
 *  odd integer values should be used as Set Cmd. and even as Get Cmd.
 */
typedef enum PN_CORE_IOCTL_CODE_Etag
{
  /** Configure SUPPORT_SNMP  behavior  */
  PN_CORE_IOCTL_SET_SNMP_SUPPORT            = 0x00010001,

  /** Placeholder for explanations of the schema that are required for internal clarification
   *  PN_CORE_IOCTL_GET_SNMP_SUPPORT        = 0x00010002,
   */
  PN_CORE_IOCTL_SET_SNMP_BULK_SUPPORT       = 0x00010003,

  /** Placeholder for explanations of the schema that are required for internal clarification
   *  PN_CORE_IOCTL_GET_SNMP_BULK_SUPPORT   = 0x00010004,
   */

}  PN_CORE_IOCTL_CODE_E;

typedef __HIL_PACKED_PRE struct PN_CORE_IOCTL_SUPPORT_Ttag
{
  /**
   *  0x0                 set support disabled
   *  0x1 ... 0xFFFFFFFF  set support enabled
   */
  uint32_t              fSupported;

} __HIL_PACKED_POST PN_CORE_IOCTL_SUPPORT_T;

typedef __HIL_PACKED_PRE struct PN_CORE_IOCTL_BLOCK_Ttag
{
  /** IOCTL  (PN_CORE_IOCTL_CODE_E)
   *  is a predefined control code that identifies the IOCTL service
   */
  uint32_t               ulCode;

  /** Additional Value, the default is 0. Depending on `ulCode`, the IOCTL services
   *  can use this to select (request) or transmit specific status data (confirmation).
   */
  uint32_t               ulAddVal;

} PN_CORE_IOCTL_BLOCK_T;


/** request */

typedef union PN_CORE_IOCTL_REQ_UNION_Ttag
{
  PN_CORE_IOCTL_SUPPORT_T                  tSetSnmp;
  PN_CORE_IOCTL_SUPPORT_T                  tSetSnmpBulkRequest;

} PN_CORE_IOCTL_REQ_UNION_T;


typedef __HIL_PACKED_PRE struct PN_CORE_IOCTL_REQ_DATA_Ttag
{
  PN_CORE_IOCTL_BLOCK_T                    tIoctl;
  PN_CORE_IOCTL_REQ_UNION_T                uReqData;

} __HIL_PACKED_POST PN_CORE_IOCTL_REQ_DATA_T;


typedef __HIL_PACKED_PRE struct PN_CORE_IOCTL_REQ_Ttag
{
  HIL_PACKET_HEADER_T                       tHead;
  PN_CORE_IOCTL_REQ_DATA_T                  tData;

} __HIL_PACKED_POST PN_CORE_IOCTL_REQ_T;


/** confirmation */

typedef union PN_CORE_IOCTL_CNF_UNION_Ttag
{
  /** union  without data-part
   * Placeholder for further IOCTL Services
   * that are required for internal processing
   */

  uint32_t               ulReserved;

} PN_CORE_IOCTL_CNF_UNION_T;

typedef __HIL_PACKED_PRE struct PN_CORE_IOCTL_CNF_DATA_Ttag
{
  PN_CORE_IOCTL_BLOCK_T                     tIcotl;
  PN_CORE_IOCTL_CNF_UNION_T                 uCnfData;

} __HIL_PACKED_POST PN_CORE_IOCTL_CNF_DATA_T;

typedef __HIL_PACKED_PRE struct PN_CORE_IOCTL_CNF_Ttag
{
  HIL_PACKET_HEADER_T                       tHead;
  PN_CORE_IOCTL_CNF_DATA_T                  tData;

} __HIL_PACKED_POST PN_CORE_IOCTL_CNF_T;


/** packet union */
typedef union PN_CORE_IOCTL_PCK_T_Ttag
{
  /*! Request Packet-based services */
  PN_CORE_IOCTL_REQ_T      tReq;
  /*! Confirmation Packet-based services */
  PN_CORE_IOCTL_CNF_T      tCnf;

} PN_CORE_IOCTL_PCK_T;

PN_ALIGNED_TYPEDEF(PN_CORE_IOCTL_BLOCK_T, 4);
PN_ALIGNED_TYPEDEF(PN_CORE_IOCTL_REQ_DATA_T, 4);
PN_ALIGNED_TYPEDEF(PN_CORE_IOCTL_CNF_DATA_T, 4);
PN_ALIGNED_TYPEDEF(PN_CORE_IOCTL_REQ_T, 4);
PN_ALIGNED_TYPEDEF(PN_CORE_IOCTL_CNF_T, 4);



/*
 * GET NEIGHBORHOOD SERVICE
 */

/* Request packet */
typedef HIL_EMPTY_PACKET_T                  PN_CORE_GET_NEIGHBORHOOD_REQ_T;

/* Confirmation packet */
typedef __HIL_PACKED_PRE struct PN_CORE_NEIGHBOR_INFO_Ttag
{
  /** the MAC address of neighbor */
  uint8_t    abMacAddress[6];
  /** the PortID of neighbor (Profinet coding) */
  uint8_t    abPortId[14];
  /** the Profinet NameOfStation ID of neighbor */
  uint8_t    abNameOfStation[240];
} __HIL_PACKED_POST PN_CORE_NEIGHBOR_INFO_T;

typedef __HIL_PACKED_PRE struct PN_CORE_GET_NEIGHBORHOOD_CNF_DATA_Ttag
{
  /** number of local ports that this confirmation returns data for */
  uint32_t ulNumberOfPorts;
  /** array of local ports containing neighborhood information */
  PN_CORE_NEIGHBOR_INFO_T tNeighbors[ARRAYS_OF_LENGTH_ZERO];
} __HIL_PACKED_POST PN_CORE_GET_NEIGHBORHOOD_CNF_DATA_T;

typedef __HIL_PACKED_PRE struct PN_CORE_GET_NEIGHBORHOOD_CNF_Ttag
{
  HIL_PACKET_HEADER_T                       tHead;
  PN_CORE_GET_NEIGHBORHOOD_CNF_DATA_T       tData;
} __HIL_PACKED_POST PN_CORE_GET_NEIGHBORHOOD_CNF_T;

/* packet union */
typedef union PN_CORE_GET_NEIGHBORHOOD_PCK_Ttag
{
  PN_CORE_GET_NEIGHBORHOOD_REQ_T                tReq;
  PN_CORE_GET_NEIGHBORHOOD_CNF_T                tCnf;
} PN_CORE_GET_NEIGHBORHOOD_PCK_T;

PN_ALIGNED_TYPEDEF(PN_CORE_GET_NEIGHBORHOOD_REQ_T, 4);
PN_ALIGNED_TYPEDEF(PN_CORE_GET_NEIGHBORHOOD_CNF_T, 4);
PN_ALIGNED_TYPEDEF(PN_CORE_GET_NEIGHBORHOOD_CNF_DATA_T, 4);


#endif /* _PN_PUBLIC_IOCTL_H_ */
