/*
 * app_config.h
 *
 *  Created on: Oct 15, 2025
 *      Author: nnguyen
 */

#ifndef CONFIG_INC_APP_CONFIG_H_
#define CONFIG_INC_APP_CONFIG_H_

/** Suppresses demo print outs
 *
 * If set the demo application will not print informations about the performed steps
 * of the application.
 *
 * Note: These print outs may impact the performance of the demo application.
 */
#define VERBOSE_1 /* Comment in to disable printouts globally */

#ifndef VERBOSE_1
  #include <stdio.h>
  #define PRINTF printf
#else
  #define PRINTF(...)
#endif

/** Defines a printf function for packet trace
 *
 * Note: you can comment out this line to deactivate packet communication print (trace) only
 * */
//#define PKT_PACKET_TRACE_PRINTF PRINTF


/** Use windows style new lines */
#define NEWLINE "\r\n"


#endif /* CONFIG_INC_APP_CONFIG_H_ */
