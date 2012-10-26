#include "ClientCommandHeartBeat.h"

using namespace QtuC;

ClientCommandHeartBeat::ClientCommandHeartBeat( QObject *parent ) : ClientCommandBase(parent)
{
	mName = "heartBeat";
	mPacketClass = packetControl;
}
