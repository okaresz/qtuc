#include "ClientCommandBase.h"

using namespace QtuC;

ClientCommandBase::ClientCommandBase( QObject *parent ) : ErrorHandlerBase(parent), mPacketClass(packetUndefined)
{}
