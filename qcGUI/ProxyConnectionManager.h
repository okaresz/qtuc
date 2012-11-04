#ifndef PROXYCONNECTIONMANAGER_H
#define PROXYCONNECTIONMANAGER_H

#include "ClientConnectionManagerBase.h"

namespace QtuC
{

class ProxyConnectionManager : public ClientConnectionManagerBase
{
	Q_OBJECT
public:
	explicit ProxyConnectionManager( QTcpSocket *socket, QObject *parent = 0 );

};

}	//QtuC::
#endif // PROXYCONNECTIONMANAGER_H
