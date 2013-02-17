#ifndef PROXYCONNECTIONMANAGER_H
#define PROXYCONNECTIONMANAGER_H

#include "ClientConnectionManagerBase.h"

namespace qcPlot
{

class ProxyConnectionManager : public QtuC::ClientConnectionManagerBase
{
	Q_OBJECT
public:
	explicit ProxyConnectionManager( QTcpSocket *socket, QObject *parent = 0 );

};

}	//QcPlot::
#endif // PROXYCONNECTIONMANAGER_H
