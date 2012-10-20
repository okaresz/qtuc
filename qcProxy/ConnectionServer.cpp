#include "ConnectionServer.h"

using namespace QtuC;

ConnectionServer::ConnectionServer() : QObject(parent = 0)
{

}

void ConnectionServer::handleNewConnection( QTcpSocket* newClient )
{

}

ClientConnectionManagerBase* ConnectionServer::getClient( QString clientID )
{

}
