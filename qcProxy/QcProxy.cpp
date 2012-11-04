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

	connect( QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(deleteLater()) );

	mDevice = new DeviceAPI( this );
	mConnectionServer = new ConnectionServer( this );
}

QcProxy::~QcProxy()
{
	debug( debugLevelVeryVerbose, "QcProxy destructor", "~QcProxy()" );
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
	/// @todo implement
	return true;
}

bool QcProxy::route(DeviceCommandBase *deviceCommand)
{	// Only call commands in the special ":proxy" hwInterface should arrive here
	/// @todo implement
	return true;
}

bool QcProxy::handleDeviceMessage(deviceMessageType_t msgType, QString msg)
{
	/// @todo implement
	debug( debugLevelVeryVerbose, QString("Device message received (%1): %2").arg( Device::messageTypeToString(msgType),msg), "handleDeviceMessage()" );
}

void QcProxy::handleNewClient( ClientConnectionManagerBase *newClient )
{
	connect( newClient, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(route(ClientCommandBase*)) );
}
