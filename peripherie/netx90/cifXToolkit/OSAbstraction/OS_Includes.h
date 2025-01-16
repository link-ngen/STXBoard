/**************************************************************************************

Copyright (c) Hilscher Gesellschaft fuer Systemautomation mbH. All Rights Reserved.

***************************************************************************************

  $Id: OS_Includes.h 6603 2014-10-02 14:57:53Z stephans $:

  Description:
    Headerfile for specific target system includes, data types and definitions

  Changes:
    Date        Description
    -----------------------------------------------------------------------------------
    2006-08-08  initial version (special OS dependencies must be added)

**************************************************************************************/

#ifndef __OS_INCLUDES__H
#define __OS_INCLUDES__H

#include <string.h>
#include <stdint.h>

/* Ignore redundant declarations to suppress warning caused by
   redundant declaration in cifXToolkit.h and cifXHWFunctions.h */
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wredundant-decls"
#endif

#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a<b)?b:a)
#define UNREFERENCED_PARAMETER(x) (x=x)
#define APIENTRY

#endif /* __OS_INCLUDES__H */
