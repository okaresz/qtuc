#include "ClientPacket.h"

using namespace QtuC;

ClientPacket::ClientPacket( const QByteArray & rawPacket )
{

}
ClientPacket::ClientPacket()
{

}

ClientPacket::ClientPacket( ClientCommandBase * clientCommand )
{

}

int ClientPacket::getIDNumber()
{

}

QString ClientPacket::getReplyTo()
{

}

packetClass ClientPacket::getClass()
{

}

void ClientPacket::setReplyTo( const QString & replyToID )
{

}

bool ClientPacket::setClass( packetClass class )
{

}

bool ClientPacket::append( ClientCommandBase clientCommand )
{

}

void ClientPacket::getPacketData() const
{

}

ClientPacket* ClientPacket::fromPacketData( const QByteArray & rawPacket )
{

}

