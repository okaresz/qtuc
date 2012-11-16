#include "QcProxy.h"
#include "ConnectionServer.h"
#include "DeviceAPI.h"
#include "ClientConnectionManagerBase.h"
#include <QCoreApplication>
#include "ProxySettingsManager.h"

using namespace QtuC;

QcProxy::QcProxy( QObject *parent ) :
	ErrorHandlerBase( parent ),
	mDevice( 0 ),
	mConnectionServer( 0 ),
	mPassThrough(false)
{
	// Create the settings object, QcProxy as parent.
    QSettings::setDefaultFormat( QSettings::IniFormat );
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
	connect( mDevice, SIGNAL(commandReceived(DeviceCommand*)), this, SLOT(route(DeviceCommand*)) );

	connect( mConnectionServer, SIGNAL( newClientConnected( ClientConnectionManagerBase * ) ), this, SLOT( handleNewClient( ClientConnectionManagerBase * ) ) );
	if( !mConnectionServer->startListening() )
	{
		error( QtCriticalMsg, "Failed to start TCP server", "start()" );
		return false;
	}
	return true;
}

void QcProxy::setPassThrough(bool pass)
{
	mPassThrough = pass;
	mDevice->setEmitAllCommand( pass );
	if( mPassThrough )
		{ debug( debugLevelInfo, "qcProxy is in PassThrough mode", "setPassThrough()" ); }
}

bool QcProxy::route(ClientCommandBase *clientCommand)
{
	ClientConnectionManagerBase *client = (ClientConnectionManagerBase*)sender();
	/// @todo implement, break apart into more subfunctions

	if( clientCommand->getClass() == ClientCommandBase::clientCommandControl )
	{
		if( clientCommand->getName() == "reqDeviceAPI" )
		{	/// @todo Permission to send API?
			client->sendCommand( new ClientCommandDeviceApi( mDevice->getApiParser()->getString() ) );
		}
		/// @todo implement
	}
	else if( clientCommand->getClass() == ClientCommandBase::clientCommandDevice )
	{
		if( mPassThrough )
		{
			if( !mDevice->command( new DeviceCommand(*(DeviceCommandBase*)clientCommand) ) )
				{ error( QtWarningMsg, QString("Failed to send command %1"), "route(ClientCommandBase*)" ); }
		}
		else
		{
			/// @todo implement
		}
	}
	debug( debugLevelVeryVerbose, QString("Route client command: %1").arg(clientCommand->getName()), "route(ClientCommandBase*)" );
	clientCommand->deleteLater();
	return true;
}

bool QcProxy::route(DeviceCommand *deviceCommand)
{
	debug( debugLevelVeryVerbose, QString("Route device command: %1").arg( DeviceCommandBuilder(*deviceCommand).getCommandString() ), "route(DeviceCommand*)" );
	if( mPassThrough )
	{
		mConnectionServer->broadcast( new ClientCommandDevice(deviceCommand) );
	}
	else	// If mPassThrough is false, only commands sent to the special ":proxy" interface should get here
	{
		/// @todo implement
	}
	deviceCommand->deleteLater();
	return true;
}

bool QcProxy::handleDeviceMessage(deviceMessageType_t msgType, QString msg)
{
	/// @todo implement
	debug( debugLevelVeryVerbose, QString("Device message received (%1): %2").arg( Device::messageTypeToString(msgType),msg), "handleDeviceMessage()" );
	return true;
}

void QcProxy::handleNewClient( ClientConnectionManagerBase *newClient )
{
	connect( newClient, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(route(ClientCommandBase*)) );
	mConnectionServer->getClient()->sendCommand( new ClientCommandDeviceApi( mDevice->getApiParser()->getString() ) );
}
