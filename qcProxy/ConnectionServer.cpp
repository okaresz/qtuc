#include "ConnectionServer.h"
#include "ClientConnectionManagerBase.h"
#include "ProxySettingsManager.h"
#include <QCoreApplication>

using namespace QtuC;

ConnectionServer::ConnectionServer(QObject *parent) : ErrorHandlerBase(parent)
{
	mTcpServer = new QTcpServer(this);
	connect( mTcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()) );
}

ConnectionServer::~ConnectionServer()
{
	if( mTcpServer && mTcpServer->isListening() )
		{ mTcpServer->close(); }
	/// @todo destroy clients (so far this is not necessary, both socket and client objects are parent of server)
}

void ConnectionServer::handleNewConnection()
{
	QTcpSocket *newClientSocket = mTcpServer->nextPendingConnection();
	if( newClientSocket )
	{
		ClientConnectionManagerBase *newClient = new ClientConnectionManagerBase(newClientSocket, this);
		// set something, because this is required for communication; this will be overwritten from settings
		QHash<QString,QString> minimalSelfInfo;
		QString fallBackId = QCoreApplication::instance()->applicationName();
		if( fallBackId.isEmpty() )
			{ fallBackId = QString("unknown_client"); }
		else
			{ fallBackId = QString( fallBackId.simplified().trimmed().replace(' ', '_').toAscii() ); }
		minimalSelfInfo.insert( QString("id"), fallBackId );
		newClient->setSelfInfo(minimalSelfInfo);
		mClients.append(newClient);
		debug( debugLevelInfo, QString("New client connected: #%1").arg(mClients.size()-1), "handleNewConnection()" );
		emit newClientConnected(newClient);
	}
}

ClientConnectionManagerBase* ConnectionServer::getClient( QString clientID )
{
	for( int i=0; i<mClients.size(); ++i )
	{
		if( mClients.at(i)->getID() == clientID )
			{ return mClients.value(i); }
	}
	return 0;
}

ClientConnectionManagerBase *ConnectionServer::getClient(int clientNumber)
{
	if( clientNumber >= mClients.size() )
		{ return 0; }
	return mClients.at(clientNumber);
}

bool ConnectionServer::startListening()
{
	if( mTcpServer->listen( QHostAddress(ProxySettingsManager::instance()->value("serverSocket/host").toString()), ProxySettingsManager::instance()->value("serverSocket/port").toInt() ) )
	{
		debug( debugLevelInfo, QString("Listening on %1, port %2.").arg( ProxySettingsManager::instance()->value("serverSocket/host").toString(), ProxySettingsManager::instance()->value("serverSocket/port").toString() ), "startListening()" );
		return true;
	}

	error( QtCriticalMsg, QString("Failed to listen on %1, port %2").arg( ProxySettingsManager::instance()->value("serverSocket/host").toString(), ProxySettingsManager::instance()->value("serverSocket/port").toString() ), "startListening()" );
	return false;
}

bool ConnectionServer::broadcast( ClientCommandBase *cmd )
{
	if( !( cmd && cmd->isValid() ) )
	{
		error( QtWarningMsg, QString( "Command (name: %1) is invalid, won't broadcast.").arg(cmd->getName()), "broadcast()" );
		return false;
	}

	bool ok = true;
	for( int i=0; i<mClients.size(); ++i )
	{
		if( !mClients.at(i) )
		{
			error( QtWarningMsg, QString("Client (index:%1) is null! Can't send command %2").arg(QString::number(i),cmd->getName()), "broadcast()" );
			continue;
		}
		if( !mClients.value(i)->sendCommand( cmd ) )
		{
			error( QtWarningMsg, QString( "Failed to send command (name: %1) to client: %2").arg(cmd->getName(), mClients.at(i)->getID()), "broadcast()" );
			ok = false;
		}
	}
	return ok;
}
