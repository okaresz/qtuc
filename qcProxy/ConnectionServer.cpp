#include "ConnectionServer.h"
#include "ClientConnectionManagerBase.h"
#include "ProxySettingsManager.h"

using namespace QtuC;

QString ConnectionServer::serverId = "qcProxy";

ConnectionServer::ConnectionServer(QObject *parent) : ErrorHandlerBase(parent)
{
	advertisedId = ProxySettingsManager::instance()->value( "serverSocket/serverId" );
	mTcpServer = new QTcpServer(this);
	connect( mTcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()) );
}

ConnectionServer::~ConnectionServer()
{
	if( mTcpServer && mTcpServer->isListening() )
		{ mTcpServer->close(); }
	/// @todo destroy clients
}

void ConnectionServer::handleNewConnection()
{
	QTcpSocket *newClientSocket = mTcpServer->nextPendingConnection();
	if( newClientSocket )
	{
		ClientConnectionManagerBase *newClient = new ClientConnectionManagerBase(newClientSocket, this);
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
	if( mTcpServer->listen( ProxySettingsManager::instance()->value("serverSocket/host"), ProxySettingsManager::instance()->value("serverSocket/port").toInt() ) )
	{
		debug( debugLevelInfo, QString("Listening on %1, port %2.").arg( ProxySettingsManager::instance()->value("serverSocket/host"), ProxySettingsManager::instance()->value("serverSocket/port") ), "startListening()" );
		return true;
	}

	error( QtCriticalMsg, QString("Failed to listen on %1, port %2").arg( ProxySettingsManager::instance()->value("serverSocket/host"), ProxySettingsManager::instance()->value("serverSocket/port") ), "startListening()" );
	return false;
}
