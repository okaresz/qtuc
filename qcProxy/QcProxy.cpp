#include "QcProxy.h"
#include "ConnectionServer.h"
#include "DeviceAPI.h"
#include "ClientConnectionManagerBase.h"

using namespace QtuC;

QcProxy::QcProxy( QObject *parent ) : ErrorHandlerBase( parent ), mDevice( 0 ), mConnectionServer( 0 )
{
	mDevice = new DeviceAPI( this );
	mConnectionServer = new ConnectionServer( this );

	if( !mDevice->initAPI() )
	{
		error( QtFatalMsg, "Failed to initialize device API", "QcProxy()" );
		return;
	}
	connect( mDevice, SIGNAL(messageReceived(deviceMessageType_t,QString)), this, SLOT(handleDeviceMessage(deviceMessageType_t,QString)) );
	connect( mDevice, SIGNAL(commandReceived(DeviceCommandBase*)), this, SLOT(route(DeviceCommandBase*)) );

	connect( mConnectionServer, SIGNAL( newClientConnected( ClientConnectionManagerBase * ) ), this, SLOT( handleNewClient( ClientConnectionManagerBase * ) ) );
	if( !mConnectionServer->startListening() )
	{
		error( QtFatalMsg, "Failed to start TCP server", "QcProxy()" );
		return;
	}
}

QcProxy::~QcProxy()
{

}

bool QcProxy::route(ClientCommandBase *clientCommand)
{

}

bool QcProxy::route(DeviceCommandBase *deviceCommand)
{
}

void QcProxy::handleNewClient( ClientConnectionManagerBase *newClient )
{
	connect( newClient, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(route(ClientCommandBase*)) );
}
