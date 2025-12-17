/*
 * netx_functions.h
 *
 *  Created on: 16.12.2025
 *      Author: nnguyen
 */

#ifndef WORKERS_INC_NETX_FUNCTIONS_H_
#define WORKERS_INC_NETX_FUNCTIONS_H_

#include "netx_worker.h"
#include "pns_functions.h"

#define REALTIME_ETH_CHANNEL 0

int32_t InitializeToolkit(NetxRessource_t *ptNetxRsc);
int32_t ProcessIOData(NetxRessource_t *ptNetxRsc);

#endif /* WORKERS_INC_NETX_FUNCTIONS_H_ */
