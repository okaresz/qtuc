#include "ProxyConnectionManager.h"
#include <QTcpSocket>
#include "PlotSettingsManager.h"

using namespace qcPlot;

ProxyConnectionManager::ProxyConnectionManager(QTcpSocket *socket, QObject *parent) :
	QtuC::ClientConnectionManagerBase( socket, false, parent )
{
	socket->setParent(this);
}
