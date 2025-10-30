/**************************************************************************************
  Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.
***************************************************************************************
  $Id:$

  Description:
    PROFINET Version

**************************************************************************************/
#ifndef PNS_FIRMWARE_VERSION_H_
#define PNS_FIRMWARE_VERSION_H_

#define PNS_FIRMWARE_VERSION_MAJOR        5
#define PNS_FIRMWARE_VERSION_MINOR        7
#define PNS_FIRMWARE_VERSION_BUILD        0
#define PNS_FIRMWARE_VERSION_REVISION     2

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#if !defined(PNS_FIRMWARE_VERSION_MAJOR) || !defined(PNS_FIRMWARE_VERSION_MINOR) || !defined(PNS_FIRMWARE_VERSION_BUILD) || !defined(PNS_FIRMWARE_VERSION_REVISION)
#error "Missing version define"
#endif

#define PNS_VERSION "PROFINET V" TOSTRING(PNS_FIRMWARE_VERSION_MAJOR) "." TOSTRING(PNS_FIRMWARE_VERSION_MINOR) "." TOSTRING(PNS_FIRMWARE_VERSION_BUILD) "." TOSTRING(PNS_FIRMWARE_VERSION_REVISION)


#endif

/*************************************************************************************/
/*-----------------------------------------------------------------------------------*/
/*-------------- EOF ----------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
/*************************************************************************************/
