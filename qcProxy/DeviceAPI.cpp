#include "DeviceAPI.h"
#include "DeviceCommand.h"
#include "SerialDeviceConnector.h"
#include "DummySocketDevice.h"
#include "DeviceAPIFileHandler.h"
#include "ProxySettingsManager.h"
#include <QDateTime>

using namespace QtuC;

DeviceAPI::DeviceAPI( QObject *parent ) :
	ErrorHandlerBase(parent),
	mEmitAllCmd(false),
	mReceivedDeviceCommandCounter(0)
{
	mStateManager = new DeviceStateManager(this);
	mDeviceAPI = new DeviceAPIFileHandler(this);
	mDeviceInstance = Device::create(this);

	mDeviceLink = new SerialDeviceConnector(this);
	//mDeviceLink = new DummySocketDevice(this);

	// set device command separator
	DeviceCommand::setSeparator( ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar() );

	// set Device Time resolution
	bool ok;
	mDeviceInstance->setDeviceTimeTicksPerMs( ProxySettingsManager::instance()->value( "device/timeTicksPerMs" ).toDouble(&ok) );
	if( !ok )
		{ error( QtWarningMsg, QString("Invalid value in config file for device/timeTicksPerMs, fallback to default(%1)").arg(QString::number(Device::getDeviceTimeTicksPerMs())), "DeviceAPI()" ); }
	else
		{ debug( debugLevelVerbose, QString("Device time resolution set to %1 ticks / ms").arg( QString::number(Device::getDeviceTimeTicksPerMs()) ), "DeviceAPI()" ); }

	connect( mStateManager, SIGNAL(stateVariableUpdateRequest(DeviceStateProxyVariable*)), this, SLOT(handleStateVariableUpdateRequest(DeviceStateProxyVariable*)) );
	connect( mStateManager, SIGNAL(stateVariableSendRequest(DeviceStateProxyVariable*)), this, SLOT(handleStateVariableSendRequest(DeviceStateProxyVariable*)) );
}

bool DeviceAPI::call( const QString &hwInterface, const QString &function, const QString &arg )
{
	DeviceCommand *dCmd = new DeviceCommand();
	dCmd->setType( deviceCmdCall );
	if( !dCmd->setInterface( hwInterface ) )
	{
		error( QtWarningMsg, "Could not set interface, command is ignored.", "call()" );
		dCmd->deleteLater();
		return false;
	}
	dCmd->setFunction( function );
	if( !dCmd->setArgumentString(arg) )
	{
		error( QtWarningMsg, "Could not set argument, command is ignored.", "call()" );
		dCmd->deleteLater();
		return false;
	}
	if( !mDeviceLink->sendCommand( dCmd ) )
	{
		error( QtWarningMsg, "Device function call failed", "call()" );
		return false;
	}
	return true;
}

DeviceStateVariableBase *DeviceAPI::getVar( const QString &hwInterface, const QString &varName )
{
	DeviceStateVariableBase *var = mStateManager->getVar( hwInterface, varName );
	if( !var )
	{
		ErrorHandlerBase::errorDetails_t errDet;
		errDet.insert( "hwInterface", hwInterface );
		errDet.insert( "varName", varName );
		error( QtWarningMsg, "No such variable", "getVar()", errDet );
	}
	return var;
}

QList<DeviceStateVariableBase*> DeviceAPI::getVarList(const QString &hardwareInterface)
{
	return mStateManager->getVarList( hardwareInterface );
}

bool DeviceAPI::update( const QString &hwInterface, const QString &varName )
{
	return handleStateVariableUpdateRequest( (DeviceStateProxyVariable*)getVar( hwInterface, varName ) );
}

bool DeviceAPI::command(DeviceCommand *cmd)
{
	if( cmd->isValid() )
		{ return mDeviceLink->sendCommand( cmd ); }
	else
	{
		error( QtWarningMsg, "Invalid command", "command()" );
		return false;
	}
}

bool DeviceAPI::set( const QString &hwInterface, const QString &varName, const QString &newVal )
{/// @todo QVariant version of this func? Also: QVariant.toString is not an elegant solution... somehow a static deviceformatter function?
	if( Device::positiveAck() )
	{
		DeviceCommand *cmd = new DeviceCommand();
		cmd->setType( deviceCmdSet);
		cmd->setInterface( hwInterface );
		cmd->setVariable( varName );
		cmd->setArgumentString( newVal );
		if( !mDeviceLink->sendCommand( cmd ) )
		{
			errorDetails_t errDet;
			errDet.insert( "posAck", "true");
			errDet.insert( "hwInterface", hwInterface );
			errDet.insert( "varName", varName );
			errDet.insert( "varVal", newVal );
			error( QtWarningMsg, "Failed to send set command", "set()", errDet );
			return false;
		}
	}
	else
	{
		DeviceStateProxyVariable* var = (DeviceStateProxyVariable*)getVar( hwInterface, varName );
		if( var )
			{ var->setValue(newVal); }
		else
		{
			errorDetails_t errDet;
			errDet.insert( "posAck", "false");
			errDet.insert( "hwInterface", hwInterface );
			errDet.insert( "varName", varName );
			errDet.insert( "varVal", newVal );
			error( QtWarningMsg, "Failed to send set command: no such variable", "set()", errDet );
			return false;
		}
	}
	return true;
}

bool DeviceAPI::initAPI( const QString &apiDefString )
{
	/// @todo Implement argument (at reinit also)
	/// @todo don't allow loading if something is alrady loaded.
	/// @todo clean up this... The duplicate load and reload messages don't look so good on the console...

	if( mDeviceInstance->isCreated() )
	{
		error( QtWarningMsg, "API already initialized and created. Use reInit()!", "initAPI()" );
		return false;
	}

	if( !apiDefString.isEmpty() )
	{
		// Connect nothing on first pass, only if API is successfully parsed
		if( !mDeviceAPI->parseAPI(apiDefString) )
		{
			error( QtCriticalMsg, "Failed to parse deviceAPI string", "initAPI()" );
			return false;
		}
		else
		{
			connect( mDeviceAPI, SIGNAL(newDeviceInfo(QString,QString)), mDeviceInstance, SLOT(setInfo(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newHardwareInterface(QString,QString)), mDeviceInstance, SLOT(addHardwareInterface(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newStateVariable(QHash<QString,QString>)), mStateManager, SLOT(registerNewStateVariable(QHash<QString,QString>)) );
			connect( mDeviceAPI, SIGNAL(newDeviceFunction(QString,QString,QString)), mDeviceInstance, SLOT(addFunction(QString,QString,QString)) );
			if( !mDeviceAPI->parseAPI(apiDefString) )
			{
				error( QtCriticalMsg, "Failed to re-parse deviceAPI string", "initAPI()" );
				return false;
			}
			mDeviceInstance->setCreated();
		}
	}
	else
	{
		// Connect nothing on first pass, only if API is successfully parsed
		if( !mDeviceAPI->load() )
		{
			error( QtCriticalMsg, "Failed to pre-load deviceAPI", "initAPI()" );
			return false;
		}
		else
		{
			connect( mDeviceAPI, SIGNAL(newDeviceInfo(QString,QString)), mDeviceInstance, SLOT(setInfo(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newHardwareInterface(QString,QString)), mDeviceInstance, SLOT(addHardwareInterface(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newStateVariable(QHash<QString,QString>)), mStateManager, SLOT(registerNewStateVariable(QHash<QString,QString>)) );
			connect( mDeviceAPI, SIGNAL(newDeviceFunction(QString,QString,QString)), mDeviceInstance, SLOT(addFunction(QString,QString,QString)) );
			if( !mDeviceAPI->reload() )
			{
				error( QtCriticalMsg, "Failed to reload deviceAPI", "initAPI()" );
				return false;
			}
			mDeviceInstance->setCreated();
		}
	}

	debug( debugLevelInfo, "deviceAPI loaded, Device created", "initAPI()" );

	// === Connect to device ================

	connect( mDeviceLink, SIGNAL(commandReceived(DeviceCommand*)), this, SLOT(handleDeviceCommand(DeviceCommand*)) );
	if( !mDeviceLink->openDevice() )
	{
		error( QtCriticalMsg, "Failed to connect to device", "initAPI()" );
		return false;
	}

	return true;
}

bool DeviceAPI::reInitAPI( const QString &apiDefString )
{
	debug( debugLevelInfo, "reinitAPI() is not implemented yet. Do nothing.", "reInitAPI()" );
	return false;

	/// @todo Implement, only reinit, if something is already running.

	// Stop everithing, or delete everything (timers, autoupdates, subscriptions....
	// reinit StateManager, stateVars
	// reparse API, file
	mDeviceLink->closeDevice();

	Device *oldDevice = mDeviceInstance;
	mDeviceInstance = 0;
	if( !initAPI(apiDefString) )
	{
		error( QtCriticalMsg, "Failed to initialize deviceAPI", "reInitAPI()" );
		mDeviceInstance = oldDevice;
		return false;
	}
	else
	{
		oldDevice->deleteLater();
		return true;
	}
}

void DeviceAPI::handleDeviceGreeting( DeviceCommand *greetingCmd )
{
	// Greeting can occur anytime, not just after proxy start as the first message, so handle accordingly...

	if( greetingCmd->hasArg() )
	{
		QHash<QString,QString> greetingInfoList;
		QHash<QString,QString> deviceInfoList = Device::getInfoList();
		QString greetingMsg;

		QStringList argList = greetingCmd->getArgList();
		for( int i=0; i< argList.size(); ++i )
		{
			QString key = argList.at(i).left( argList.at(i).indexOf(':') );
			QString val = argList.at(i).mid( argList.at(i).indexOf(':')+1 );
			if( key == "msg" )
				{ greetingMsg = val; }
			else
				{ greetingInfoList.insert( key, val ); }
		}

		/// @todo This should reach the clients as well!
		if( !greetingMsg.isEmpty() )
			{ debug( debugLevelInfo, QString("Device greeting received: %1").arg( greetingMsg ), "handleDeviceGreeting()" ); }

		// If something is different than the stored info, we must update the Device object
		bool isDiff = false;
		QHash<QString,QString>::const_iterator info = greetingInfoList.constBegin();
		while( info != greetingInfoList.constEnd() )
		{
			if( !deviceInfoList.contains( info.key() ) )
			{
				isDiff = true;
				break;
			}
			else
			{
				if( deviceInfoList.value( info.key() ) != greetingInfoList.value( info.key() ) )
				{
					isDiff = true;
					break;
				}
			}
			++info;
		}

		// ...then update
		if( isDiff )
		{
			// If name or platform is different, notify the user that the device has changed
			/// @todo This should reach the clients as well!
			if( !deviceInfoList.value("name").isEmpty() && deviceInfoList.value("name") != greetingInfoList.value("name") )
				{ error( QtWarningMsg, QString("Device name (%1) differs from the one in deviceAPI (%2), possible device<->API mismatch!").arg( greetingInfoList.value("name"), deviceInfoList.value("name") ), "handleDeviceGreeting()" ); }
			if( !deviceInfoList.value("platform").isEmpty() && deviceInfoList.value("platform") != greetingInfoList.value("platform") )
				{ error( QtWarningMsg, QString("Device platform (%1) differs from the one in deviceAPI (%2), possible device<->API mismatch!").arg( greetingInfoList.value("platform"), deviceInfoList.value("platform") ), "handleDeviceGreeting()" ); }

			// unlock Device to update
			mDeviceInstance->setCreated( false );

			info = greetingInfoList.constBegin();
			while( info != greetingInfoList.constEnd() )
			{
				mDeviceInstance->setInfo( info.key(), info.value() );
				++info;
			}

			// lock device
			mDeviceInstance->setCreated();
		}

		// update startup time. This must be after the device info has been parsed from the greeting message (timeTicksPerMs must be set).
		if( greetingCmd->hasTimestamp() )
		{
			mDeviceInstance->setCreated( false );
			mDeviceInstance->setStartupTime( QDateTime::currentMSecsSinceEpoch() - Device::timeStampToMs(greetingCmd->getTimestamp()) );
			mDeviceInstance->setCreated();
		}
	}
	else /// @todo This should reach the clients as well!
		{ debug( debugLevelInfo, "Device greeting received (empty greeting)", "handleDeviceGreeting()" ); }

	greetingCmd->deleteLater();
}

void DeviceAPI::handleDeviceCommand( DeviceCommand *cmd )
{
	++mReceivedDeviceCommandCounter;

	if( mEmitAllCmd )
	{
		emit commandReceived( cmd );
		return;
	}

	// If this is the first command, try to set device startup time
	// this should run, even if this is a greeting message (handleDeviceGreeting() only sets startupTime if the greeting has a timestamp)
	if( mReceivedDeviceCommandCounter == 1 && !Device::getStartupTime() )
	{
		qint64 startupTime = 0;
		if( cmd->hasTimestamp() )	// if command has timestamp, use it
			{ startupTime = QDateTime::currentMSecsSinceEpoch() - Device::timeStampToMs(cmd->getTimestamp()); }
		else	// otherwise device startup time will be the current time
			{ startupTime = QDateTime::currentMSecsSinceEpoch(); }

		mDeviceInstance->setCreated( false );
		mDeviceInstance->setStartupTime( startupTime );
		mDeviceInstance->setCreated();
	}

	if( cmd->getType() == deviceCmdCall )
	{
		if( cmd->getHwInterface() == ":proxy" )
		{
			if( cmd->getVariable() == "message" )
			{
				emit messageReceived( Device::messageTypeFromString(cmd->getArg(0)), QStringList( cmd->getArgList().mid(1) ).join(" ") );
				cmd->deleteLater();
			}
			else if( cmd->getVariable() == "greeting" )
			{
				handleDeviceGreeting( cmd );
			}
			else
				{ emit commandReceived( cmd ); }
		}
		else
			{ cmd->deleteLater(); }
	}
	else
	{
		if( cmd->getType() == deviceCmdGet )
		{
			DeviceCommand *setCmd = DeviceCommand::fromVariable( deviceCmdSet, (DeviceStateProxyVariable*)mStateManager->getVar( cmd->getHwInterface(), cmd->getVariable() ) );
			if( !mDeviceLink->sendCommand( setCmd ) )
				{ error( QtWarningMsg, "Failed to reply to device get", "handleDeviceCommand()" ); }
		}
		else if( cmd->getType() == deviceCmdSet )
		{
			DeviceStateProxyVariable *var = (DeviceStateProxyVariable*)mStateManager->getVar( cmd->getHwInterface(), cmd->getVariable() );

			qint64 updateTime = 0;
			if( cmd->hasTimestamp() )	// if command has timestamp, use it
				{ updateTime = Device::timeStampToUnix(cmd->getTimestamp()); }
			else
				{ updateTime = QDateTime::currentMSecsSinceEpoch(); }

			if( var )
				{ var->updateFromDevice( cmd->getArg(), updateTime ); }
			else
				{ error( QtWarningMsg, QString("Failed to set variable from device, no such variable (hwI: %1, name: %2)").arg(cmd->getHwInterface(),cmd->getVariable()), "handleDeviceCommand()" ); }
		}
		else
			{ error( QtWarningMsg, "Got an undefined command?! Weird...", "handleDeviceCommand()" ); }
		cmd->deleteLater();
	}
}

bool DeviceAPI::handleStateVariableUpdateRequest(DeviceStateProxyVariable *stateVar)
{
	if( !mDeviceLink->sendCommand( DeviceCommand::fromVariable( deviceCmdGet, stateVar ) ) )
	{
		error( QtWarningMsg, QString("Failed to update stateVar: %1").arg(stateVar->getName()), "handleStateVariableUpdateRequest()" );
		return false;
	}
	return true;
}

void DeviceAPI::handleStateVariableSendRequest(DeviceStateProxyVariable *stateVar)
{
	if( !mDeviceLink->sendCommand( DeviceCommand::fromVariable( deviceCmdSet, stateVar ) ) )
		{ error( QtWarningMsg, QString("Failed to send %1:%2 to device").arg(stateVar->getHwInterface(),stateVar->getName()), "handleSetVariableSendRequest()" ); }
}
