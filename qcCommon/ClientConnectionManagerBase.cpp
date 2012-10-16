#include "ClientConnectionManagerBase.h"

using namespace QtuC;

ClientConnectionManagerBase::ClientConnectionManagerBase()
{

}

void ClientConnectionManagerBase::commandReceived( ClentCommandBase cmd )
{

}

bool ClientConnectionManagerBase::sendCommand( ClentCommandBase * cmd )
{

}

bool ClientConnectionManagerBase::sendCommands( QList<ClentCommandBase*> cmdList )
{

}

bool ClientConnectionManagerBase::isConnected() const
{

}

ClientPacket * ClientConnectionManagerBase::pack( ClientCommandBase * cmd )
{

}

ClientPacket * ClientConnectionManagerBase::pack( QList<ClientCmmandBase*> cmdList )
{

}

bool ClientConnectionManagerBase::isReady() const
{

}

const QString ClientConnectionManagerBase::getID() const
{

}

void ClientConnectionManagerBase::sendHandShake()
{

}

void ClientConnectionManagerBase::ackHeartBeat( ClientCommandHeartBeat* hb )
{

}

ConnectionState ClientConnectionManagerBase::getState() const
{

}

const QHash<QString,QString> ClientConnectionManagerBase::getClientInfo()
{

}

void ClientConnectionManagerBase::setSelfInfo( const QString & key, const QString & value )
{

}

bool ClientConnectionManagerBase::cmdHeartBeat( qint64 ackID )
{

}

bool ClientConnectionManagerBase::cmdHandShake()
{

}

void ClientConnectionManagerBase::cmdSendDeviceAPIDef( const QString& apiDefString )
{

}

