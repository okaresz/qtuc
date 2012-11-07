#include "QcGui.h"
#include "GuiSettingsManager.h"
#include <QCoreApplication>

using namespace QtuC;

QcGui::QcGui(QObject *parent) : ErrorHandlerBase(parent)
{
	// create settings
	GuiSettingsManager::instance(this);

	mProxyState = new ProxyStateManager(this);
}

QcGui::~QcGui()
{
	debug( debugLevelVeryVerbose, "QcGui destroyed", "~QcGui()" );
}

void QcGui::connectProxy()
{
	// give ClientConnectionManagerBase the self-info
	QHash<QString,QString> selfInfo;
	selfInfo.insert( QString("id"), GuiSettingsManager::instance()->value( "selfInfo/id" ).toString() );
	selfInfo.insert( QString("name"), GuiSettingsManager::instance()->value( "selfInfo/name" ).toString() );
	selfInfo.insert( QString("desc"), GuiSettingsManager::instance()->value( "selfInfo/desc" ).toString() );
	selfInfo.insert( QString("author"), GuiSettingsManager::instance()->value( "selfInfo/author" ).toString() );
	selfInfo.insert( QString("version"), QCoreApplication::instance()->applicationVersion() );
	ClientConnectionManagerBase::setSelfInfo(selfInfo);

	QTcpSocket *proxySocket = new QTcpSocket(0);

	connect( proxySocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(proxyConnectError()) );
	connect( proxySocket, SIGNAL(connected()), this, SLOT(proxyConnected()) );
	proxySocket->connectToHost( GuiSettingsManager::instance()->value("proxyAddress/host").toString(), GuiSettingsManager::instance()->value("proxyAddress/port").toInt() );
}

void QcGui::proxyConnectError()
{
	errorDetails_t errDet;
	errDet.insert( "error", ((QTcpSocket*)sender())->errorString() );
	error( QtCriticalMsg, "Failed to connect to proxy", "proxyConnectError", errDet );
	delete sender();
}

void QcGui::proxyConnected()
{
	mProxyLink = new ProxyConnectionManager( (QTcpSocket*)sender(), this );

	// disconnect socket from me, ProxyConnectionManager is in control now
	disconnect( sender(), 0, this, 0 );

	mProxyLink->sendHandShake();

	connect( mProxyLink, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(handleCommand(ClientCommandBase*)) );
}

void QcGui::proxyDisconnected()
{
	mProxyLink->disconnect();
	debug( debugLevelInfo, "Proxy disconnected", "proxyDisconnected()" );
}

void QcGui::handleCommand(ClientCommandBase *cmd)
{
	if( cmd->getClass() == ClientCommandBase::clientCommandDevice )
	{
		ClientCommandDevice *ccd = (ClientCommandDevice*)cmd;
		debug( debugLevelInfo, QString("Device command received: %1 %2 %3 %4").arg(ccd->getName(), ccd->getHwInterface(), ccd->getVariable(), ccd->getArg() ), "handleCommand()" );
	}
	else	//control
	{
		if( cmd->getName() == "deviceAPI" )
		{
			ClientCommandDeviceApi *apiCmd = (ClientCommandDeviceApi*)cmd;
			debug( debugLevelInfo, QString("API received: %1 | %2").arg(QString(apiCmd->getHash()),QString(apiCmd->getEncodedApi())), "handleCommand()" );
		}
		else
			{ debug( debugLevelInfo, QString("Command received: %1").arg(cmd->getName()), "handleCommand()" ); }
	}
	cmd->deleteLater();
}
