#ifndef CLIENTCOMMANDS_H
#define CLIENTCOMMANDS_H

/** @file
 *	This header collects the various ClientCommand declaration headers.
 *	@note Don't forget to register the command in QtuC::ClientCommandFactory!*/

#include "ClientCommandHandshake.h"
#include "ClientCommandHeartBeat.h"
#include "ClientCommandReqDeviceApi.h"
#include "ClientCommandDeviceApi.h"
#include "ClientCommandDevice.h"
#include "ClientCommandSubscribe.h"
#include "ClientCommandUnSubscribe.h"
class ClientCommandStatus;

#endif // CLIENTCOMMANDS_H
