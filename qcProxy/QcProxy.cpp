#include "QcProxy.h"
#include "ConnectionServer.h"
#include "DeviceAPI.h"
#include "ClientConnectionManagerBase.h"
#include <QCoreApplication>
#include "ProxySettingsManager.h"
#include <QFile>
#include <QTextStream>

using namespace QtuC;

QcProxy::QcProxy( QObject *parent ) :
	ErrorHandlerBase( parent ),
	mDevice( 0 ),
	mConnectionServer( 0 ),
	mClientSubscriptionManager(0),
	mPassThrough(false)
{
	connect( QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(deleteLater()) );

	mDevice = new DeviceAPI( this );
	mConnectionServer = new ConnectionServer( this );
	mClientSubscriptionManager = new ClientSubscriptionManager(this);

	connect( mClientSubscriptionManager, SIGNAL(subscriptionFeedRequest(ClientSubscription*)), this, SLOT(sendSubscriptionFeed(ClientSubscription*)) );
}

QcProxy::~QcProxy()
{}

bool QcProxy::start()
{
	setPassThrough( ProxySettingsManager::instance()->getCmdArgValue(ProxySettingsManager::cmdArgPassthrough).toBool() );

	if( !mDevice->initAPI() )
	{
		error( QtCriticalMsg, "Failed to initialize device", "start()" );
		return false;
	}
	connect( mDevice, SIGNAL(messageReceived(deviceMessageType_t,QString)), this, SLOT(handleDeviceMessage(deviceMessageType_t,QString)) );
	connect( mDevice, SIGNAL(commandReceived(DeviceCommand*)), this, SLOT(route(DeviceCommand*)) );
	connect( mDevice, SIGNAL(greetingReceived()), this, SLOT(handleDeviceGreeting()) );

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
	/// @todo implement, break apart into more subfunctions, return false on error, but don't forget to delete the command at the end: solution: sg. like: ClientCommandBase cmd = &*clientCommand; ? no copy but on the stack->implicit delete: check!

	if( clientCommand->getClass() == ClientCommandBase::clientCommandControl )
	{
		if( clientCommand->getName() == "reqDeviceAPI" )
		{	/// @todo Permission to send API?
			debug( debugLevelVeryVerbose, "Got API request, send API...", "route(ClientCommandBase*)" );
			client->sendCommand( new ClientCommandDeviceApi( mDevice->getApiParser()->getApiString() ) );
		}
		else if( clientCommand->getName() == "subscribe" )
		{
			/// @todo ignore when in passthrough mode
			ClientCommandSubscribe *subscribeCmd = (ClientCommandSubscribe*)clientCommand;
			mClientSubscriptionManager->subscribe( client, subscribeCmd->getInterval(), subscribeCmd->getHwInterface(), subscribeCmd->getVariable() );
		}
		else if( clientCommand->getName() == "unSubscribe" )
		{
			/// @todo ignore when in passthrough mode
			ClientCommandUnSubscribe *unSubscribeCmd = (ClientCommandUnSubscribe*)clientCommand;
			mClientSubscriptionManager->unSubscribe( client, unSubscribeCmd->getHwInterface(), unSubscribeCmd->getVariable() );
		}
		else if( clientCommand->getName() == "reqDeviceInfo" )
		{
			ClientCommandDeviceInfo *cmdInfo = new ClientCommandDeviceInfo();
			cmdInfo->setStartup(false);
			cmdInfo->setStartupTime( Device::getStartupTime() );
			cmdInfo->setPositiveAck( Device::positiveAck() );
			cmdInfo->setInfoList( Device::getInfoList() );
			client->sendCommand( cmdInfo );
		}
		else
			{ error( QtWarningMsg, QString("Unimplemented control clientCommand received: %1").arg(clientCommand->getName()), "route(ClientCommandBase*)" ); }
	}
	else if( clientCommand->getClass() == ClientCommandBase::clientCommandDevice )
	{
		DeviceCommandBase *clientCommandDevice = (ClientCommandDevice*)clientCommand;
		if( mPassThrough )
		{
			if( !mDevice->command( new DeviceCommand(*clientCommandDevice) ) )
			{
				error( QtWarningMsg, QString("Failed to send device command %1").arg(DeviceCommandBase::commandTypeToString(clientCommandDevice->getType())), "route(ClientCommandBase*)" );
			}
		}
		else
		{
			switch( clientCommandDevice->getType() )
			{
				case deviceCmdGet:
				{
					if( !client->sendCommand( new ClientCommandDevice( deviceCmdSet, mDevice->getVar( clientCommandDevice->getHwInterface(), clientCommandDevice->getVariable() ) ) ) )
						{ error( QtWarningMsg, QString("Failed to send set device command to %1").arg(client->getID()), "route(ClientCommandBase*)" ); }
				}
					break;
				case deviceCmdSet:
				{
					mDevice->set( clientCommandDevice->getHwInterface(), clientCommandDevice->getVariable(), clientCommandDevice->getArg() );
				}
					break;
				case deviceCmdCall:
				{
					/// @todo separate args
					mDevice->call( clientCommandDevice->getHwInterface(), clientCommandDevice->getFunction(), clientCommandDevice->getArgList().join(" ") );
				}
					break;
				case deviceCmdUndefined:
					error( QtWarningMsg, "ClientCommandDevice of Undefined type has been received", "route(ClientCommandBase*)" );
					break;
				default:
					error( QtWarningMsg, "Huh?", "route(ClientCommandBase*)" );
			}
		}
	}
	else
		{ error( QtWarningMsg, "Class of undefined clientCommand received", "route(ClientCommandBase*)" ); }

	debug( debugLevelVeryVerbose, QString("Route client command: %1").arg(clientCommand->getName()), "route(ClientCommandBase*)" );
	clientCommand->deleteLater();
	return true;
}

bool QcProxy::route( DeviceCommand *deviceCommand )
{
	debug( debugLevelVeryVerbose, QString("Route device command: %1").arg( deviceCommand->getCommandString() ), "route(DeviceCommand*)" );
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

bool QcProxy::handleDeviceMessage(deviceMessageType_t msgType, QString const &msg)
{
	/// @todo implement sending to clients, and clean up this logging thing...

	// log
	QString logFilePath;
	switch( msgType )
	{
		case deviceMsgInfo: logFilePath = ProxySettingsManager::instance()->value("deviceLog/infoLogPath").toString(); break;
		case deviceMsgDebug: logFilePath = ProxySettingsManager::instance()->value("deviceLog/debugLogPath").toString(); break;
		case deviceMsgError: logFilePath = ProxySettingsManager::instance()->value("deviceLog/errorLogPath").toString(); break;
		default:
			error( QtWarningMsg, "Unknown device message type", "handleDeviceMessage()" );
			return false;
	}

	QFile logFile( logFilePath );
	if (!logFile.open(QIODevice::Append | QIODevice::Text))
		{ error( QtWarningMsg, "Failed to open logfile", "handleDeviceMessage()" ); }
	else
	{
		QTextStream fileStream(&logFile);
		fileStream<<msg<<"\n";
		fileStream.flush();
		logFile.close();
	}

	//debug( debugLevelVeryVerbose, QString("Device message received (%1): %2").arg( Device::messageTypeToString(msgType),msg), "handleDeviceMessage()" );

	return true;
}

void QcProxy::handleDeviceGreeting()
{
	ClientCommandDeviceInfo *cmdInfo = new ClientCommandDeviceInfo();
	cmdInfo->setStartup(true);
	cmdInfo->setStartupTime( Device::getStartupTime() );
	cmdInfo->setPositiveAck( Device::positiveAck() );
	cmdInfo->setInfoList( Device::getInfoList() );
	mConnectionServer->broadcast( cmdInfo );
}

void QcProxy::handleNewClient( ClientConnectionManagerBase *newClient )
{
	connect( newClient, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(route(ClientCommandBase*)) );
}

void QcProxy::sendSubscriptionFeed( ClientSubscription *subscription )
{
	if( subscription->getVariable().isEmpty() )
	{
		QList<DeviceStateVariableBase*> varList = mDevice->getVarList( subscription->getHwInterface() );
		QList<ClientCommandBase*> clientCmdList;

		for( int i=0; i<varList.size(); ++i )
		{
			// Don't send uninitialized and invalid variables
			if( !(!varList.at(i)->isNull() && varList.at(i)->isValid()) )
				{ continue; }
			// Don't send if a more specific subscription is available
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
		DeviceStateVariableBase *stateVar = mDevice->getVar( subscription->getHwInterface(), subscription->getVariable() );

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
