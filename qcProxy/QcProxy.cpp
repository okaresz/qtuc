#include "QcProxy.h"
#include "ConnectionServer.h"
#include "DeviceAPI.h"
#include "ClientConnectionManagerBase.h"
#include <QCoreApplication>
#include "ProxySettingsManager.h"

using namespace QtuC;

QcProxy::QcProxy( QObject *parent ) : ErrorHandlerBase( parent ), mDevice( 0 ), mConnectionServer( 0 )
{
	// Create the settings object, QcProxy as parent.
	ProxySettingsManager::instance( this );

	mDevice = new DeviceAPI( this );
	mConnectionServer = new ConnectionServer( this );
}

QcProxy::~QcProxy()
{
	debug( debugLevelInfo, "QcProxy destructor", "~QcProxy()" );
}

bool QcProxy::start()
{
	if( !mDevice->initAPI() )
	{
		error( QtCriticalMsg, "Failed to initialize device API", "start()" );
		return false;
	}
	connect( mDevice, SIGNAL(messageReceived(deviceMessageType_t,QString)), this, SLOT(handleDeviceMessage(deviceMessageType_t,QString)) );
	connect( mDevice, SIGNAL(commandReceived(DeviceCommandBase*)), this, SLOT(route(DeviceCommandBase*)) );

	connect( mConnectionServer, SIGNAL( newClientConnected( ClientConnectionManagerBase * ) ), this, SLOT( handleNewClient( ClientConnectionManagerBase * ) ) );
	if( !mConnectionServer->startListening() )
	{
		error( QtCriticalMsg, "Failed to start TCP server", "start()" );
		return false;
	}
	return true;
}

bool QcProxy::route(ClientCommandBase *clientCommand)
{

}

bool QcProxy::route(DeviceCommandBase *deviceCommand)
{
}

bool QcProxy::handleDeviceMessage(deviceMessageType_t msgType, QString msg)
{
	/// @todo implement
}

void QcProxy::handleNewClient( ClientConnectionManagerBase *newClient )
{
	QHash<QString,QString> selfInfo;
	selfInfo.insert( QString("id"), ProxySettingsManager::instance()->value( "serverInfo/id" ).toString() );
	selfInfo.insert( QString("id"), ProxySettingsManager::instance()->value( "serverInfo/name" ).toString() );
	selfInfo.insert( QString("id"), ProxySettingsManager::instance()->value( "serverInfo/desc" ).toString() );
	selfInfo.insert( QString("id"), ProxySettingsManager::instance()->value( "serverInfo/author" ).toString() );
	selfInfo.insert( QString("id"), QCoreApplication::instance()->applicationVersion() );
	newClient->setSelfInfo(selfInfo);

	connect( newClient, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(route(ClientCommandBase*)) );
}
