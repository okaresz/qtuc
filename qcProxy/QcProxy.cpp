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

void QcProxy::handleNewClient( ClientConnectionManagerBase *newClient )
{

}
