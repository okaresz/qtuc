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
	mClientSubscriptionManager(0),
	mPassThrough(false)
{
	// On windows default format is registry, but we want file.
	#ifdef Q_OS_WIN32
		QSettings::setDefaultFormat( QSettings::IniFormat );
	#endif
	// Create the settings object, QcProxy as parent.
	ProxySettingsManager::instance( this );

	connect( QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(deleteLater()) );

	mDevice = new DeviceAPI( this );
	mConnectionServer = new ConnectionServer( this );
	mClientSubscriptionManager = new ClientSubscriptionManager(this);

	connect( mClientSubscriptionManager, SIGNAL(subscriptionFeedRequest(ClientSubscription*)), this, SLOT(sendSubscriptionFeed(ClientSubscription*)) );
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
			debug( debugLevelVeryVerbose, "Got API request, send API...", "route(ClientCommandBase*)" );
			client->sendCommand( new ClientCommandDeviceApi( mDevice->getApiParser()->getString() ) );
		}
		else if( clientCommand->getName() == "subscribe" )
		{
			ClientCommandSubscribe *subscribeCmd = (ClientCommandSubscribe*)clientCommand;
			mClientSubscriptionManager->subscribe( client, subscribeCmd->getInterval(), subscribeCmd->getHwInterface(), subscribeCmd->getVariable() );
		}
		else if( clientCommand->getName() == "unSubscribe" )
		{
			ClientCommandUnSubscribe *unSubscribeCmd = (ClientCommandUnSubscribe*)clientCommand;
			mClientSubscriptionManager->unSubscribe( client, unSubscribeCmd->getHwInterface(), unSubscribeCmd->getVariable() );
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
}

void QcProxy::sendSubscriptionFeed( ClientSubscription *subscription )
{
	if( subscription->getVariable().isEmpty() )
	{
		QList<DeviceStateVariable*> varList = mDevice->getVarList( subscription->getHwInterface() );
		QList<ClientCommandBase*> clientCmdList;

		for( int i=0; i<varList.size(); ++i )
		{
			// Don't send uninitialized and invalid variables
			if( !(!varList.at(i)->isNull() && varList.at(i)->isValid()) )
				{ continue; }
			if( !mClientSubscriptionManager->moreSpecificSubscriptionExists( varList.at(i), subscription ) )
				{ clientCmdList.append( new ClientCommandDevice( deviceCmdSet, varList.at(i) ) ); }
		}

		if( clientCmdList.isEmpty() )
			{ return; }

		if( !subscription->getClient()->sendCommands( clientCmdList ) )
		{
			errorDetails_t errDet;
			errDet.insert( "subscriptionVar", subscription->getVariable() );
			errDet.insert( "subscriptionInterface", subscription->getHwInterface() );
			error( QtWarningMsg, QString("An error occured while sending subscription update to client: %1").arg(subscription->getClient()->getID()), "sendSubscriptionFeed()", errDet );
		}
	}
	else
	{
		DeviceStateVariable *stateVar = mDevice->getVar( subscription->getHwInterface(), subscription->getVariable() );

		// Don't send uninitialized and invalid variables
		if( !(!stateVar->isNull() && stateVar->isValid()) )
			{ return; }

		if( !subscription->getClient()->sendCommand( new ClientCommandDevice( deviceCmdSet, stateVar ) ) )
		{
			errorDetails_t errDet;
			errDet.insert( "subscriptionVar", subscription->getVariable() );
			errDet.insert( "subscriptionInterface", subscription->getHwInterface() );
			error( QtWarningMsg, QString("An error occured while sending subscription feed to client: %1").arg(subscription->getClient()->getID()), "sendSubscriptionFeed()", errDet );
		}
	}
}
