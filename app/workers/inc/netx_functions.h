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

int32_t InitializeToolkit(NetxRessource_t *ptNetxRsc);
int32_t ProcessIOData(NetxRessource_t *ptNetxRsc);

int32_t NetX_InitializeChannels(NetxRessource_t *ptNetxRsc, char *szBoardName);
int32_t NetX_ConfigureChannels(NetxRessource_t* ptNetxRsc);
void    NetX_CallCommMailboxRoutine(NetxRessource_t* ptNetxRsc);
void    NetX_AllChannels_Close(NetxRessource_t* ptNetxRsc);
uint32_t Netx_ReadNetworkState(NetxRessource_t *ptNetxRsc);

#endif /* WORKERS_INC_NETX_FUNCTIONS_H_ */
