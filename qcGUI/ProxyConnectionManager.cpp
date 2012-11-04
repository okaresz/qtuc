#include "ProxyConnectionManager.h"
#include <QTcpSocket>
#include "GuiSettingsManager.h"

using namespace QtuC;

ProxyConnectionManager::ProxyConnectionManager(QTcpSocket *socket, QObject *parent) :
	ClientConnectionManagerBase( socket, false, parent )
{
	socket->setParent(this);
}
