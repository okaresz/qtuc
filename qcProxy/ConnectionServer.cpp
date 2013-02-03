#include "ConnectionServer.h"
#include "ClientConnectionManagerBase.h"
#include "ProxySettingsManager.h"
#include <QCoreApplication>

using namespace QtuC;

ConnectionServer::ConnectionServer(QObject *parent) : ErrorHandlerBase(parent)
{
	mTcpServer = new QTcpServer(this);

	QHash<QString,QString> selfInfo;
	selfInfo.insert( QString("id"), ProxySettingsManager::instance()->value( "serverInfo/id" ).toString() );
	selfInfo.insert( QString("name"), ProxySettingsManager::instance()->value( "serverInfo/name" ).toString() );
	selfInfo.insert( QString("desc"), ProxySettingsManager::instance()->value( "serverInfo/desc" ).toString() );
	selfInfo.insert( QString("author"), ProxySettingsManager::instance()->value( "serverInfo/author" ).toString() );
	selfInfo.insert( QString("version"), QCoreApplication::instance()->applicationVersion() );
	ClientConnectionManagerBase::setSelfInfo(selfInfo);

	connect( mTcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()) );
}

ConnectionServer::~ConnectionServer()
{
	if( mTcpServer && mTcpServer->isListening() )
		{ mTcpServer->close(); }
	/// @todo destroy clients (so far this is not necessary, both socket and client objects are parent of server), but we should disconnect them first
}

void ConnectionServer::handleNewConnection()
{
	QTcpSocket *newClientSocket = mTcpServer->nextPendingConnection();
	if( newClientSocket )
	{
		debug( debugLevelInfo, QString("New client connection, index: #%1").arg(mClients.size()), "handleNewConnection()" );
		ClientConnectionManagerBase *newClient = new ClientConnectionManagerBase(newClientSocket, true, this);
		mClients.append(newClient);
		connect( newClient, SIGNAL(clientDisconnected()), this, SLOT(handleClientDisconnect()) );
		emit newClientConnected(newClient);
	}
}

void ConnectionServer::handleClientDisconnect()
{
	// who dares!?...
	int clientIndex = 0;
	bool found = false;
	for(; clientIndex<mClients.size(); ++clientIndex )
	{
		if( mClients.at(clientIndex) == sender() )
		{
			found = true;
			break;
		}
	}

	if( !found )
		{ error( QtWarningMsg, "Unknown client disconnected... who was it?...", "handleClientDisconnect()" ); }
	else
	{
		debug( debugLevelInfo, QString("Client %2 (#%1) disconnected").arg(QString::number(clientIndex), mClients.at(clientIndex)->getID()), "handleClientDisconnect()" );
		mClients[clientIndex]->deleteLater();
		mClients.removeAt(clientIndex);
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
	// duh, why can't QHostAddress do this?
	QString address = ProxySettingsManager::instance()->value("serverSocket/host").toString();
	if( address == "localhost" )
		{ address = "127.0.0.1"; }
	if( mTcpServer->listen( QHostAddress(address), ProxySettingsManager::instance()->value("serverSocket/port").toInt() ) )
	{
		debug( debugLevelInfo, QString("Listening on %1, port %2.").arg( ProxySettingsManager::instance()->value("serverSocket/host").toString(), ProxySettingsManager::instance()->value("serverSocket/port").toString() ), "startListening()" );
		return true;
	}

	errorDetails_t errDet;
	errDet.insert( "error", mTcpServer->errorString() );
	error( QtCriticalMsg, QString("Failed to listen on %1, port %2").arg( ProxySettingsManager::instance()->value("serverSocket/host").toString(), ProxySettingsManager::instance()->value("serverSocket/port").toString() ), "startListening()", errDet );
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
