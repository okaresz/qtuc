#include "DeviceAPI.h"
#include "DeviceCommand.h"
#include "SerialDeviceConnector.h"
#include "DummySocketDevice.h"
#include "DeviceAPIFileHandler.h"

using namespace QtuC;

DeviceAPI::DeviceAPI( QObject *parent ) :
	ErrorHandlerBase(parent),
	mEmitAllCmd(false)
{
	mStateManager = new DeviceStateManager(this);
	mDeviceAPI = new DeviceAPIFileHandler(this);
	mDeviceInstance = 0;

	//mDeviceLink = new SerialDeviceConnector(this);
	mDeviceLink = new DummySocketDevice(this);

	connect( mStateManager, SIGNAL(stateVariableUpdateRequest(DeviceStateVariable*)), this, SLOT(handleStateVariableUpdateRequest(DeviceStateVariable*)) );
}

bool DeviceAPI::call( const QString &hwInterface, const QString &function, const QString &arg )
{
	DeviceCommandBuilder *dCmd = new DeviceCommandBuilder();
	dCmd->setType( deviceCmdCall );
	dCmd->setInterface( hwInterface );
	dCmd->setFunction( function );
	dCmd->setArgumentString(arg);
	if( !mDeviceLink->sendCommand( dCmd ) )
	{
		error( QtWarningMsg, "Device function call failed", "call()" );
		return false;
	}
	return true;
}

DeviceStateVariable *DeviceAPI::getVar( const QString &hwInterface, const QString &varName )
{
	DeviceStateVariable *var = mStateManager->getVar( hwInterface, varName );
	if( !var )
	{
		ErrorHandlerBase::errorDetails_t errDet;
		errDet.insert( "hwInterface", hwInterface );
		errDet.insert( "varName", varName );
		error( QtWarningMsg, "No such variable", "getVar()", errDet );
	}
	return var;
}

QList<DeviceStateVariable *> DeviceAPI::getVarList(const QString &hardwareInterface)
{
	return mStateManager->getVarList( hardwareInterface );
}

bool DeviceAPI::update( const QString &hwInterface, const QString &varName )
{
	if( !mDeviceLink->sendCommand( DeviceCommand::build( deviceCmdGet, getVar( hwInterface, varName ) ) ) )
	{
		errorDetails_t errDet;
		errDet.insert( "hwInterface", hwInterface );
		errDet.insert( "varName", varName );
		error( QtWarningMsg, "Failed to send get command-", "update()", errDet );
		return false;
	}
	return true;
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

bool DeviceAPI::set( const QString &hwInterface, const QString &varName, const QVariant &newVal )
{
	if( Device::positiveAck() )
	{
		/// @todo this QVariant.toString is not an elegant solution... somehow a static deviceformatter function?
		DeviceCommandBuilder *cmd = new DeviceCommandBuilder();
		cmd->setType( deviceCmdSet);
		cmd->setInterface( hwInterface );
		cmd->setVariable( varName );
		cmd->setArgumentString( newVal.toString() );
		if( !mDeviceLink->sendCommand( cmd ) )
		{
			errorDetails_t errDet;
			errDet.insert( "posAck", "true");
			errDet.insert( "hwInterface", hwInterface );
			errDet.insert( "varName", varName );
			errDet.insert( "varVal", newVal.toString() );
			error( QtWarningMsg, "Failed to send set command", "set()", errDet );
			return false;
		}
	}
	else
	{
		DeviceStateVariable* var = getVar( hwInterface, varName );
		if( var )
			{ var->setValue(newVal); }
		else
		{
			errorDetails_t errDet;
			errDet.insert( "posAck", "false");
			errDet.insert( "hwInterface", hwInterface );
			errDet.insert( "varName", varName );
			errDet.insert( "varVal", newVal.toString() );
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

	if( mDeviceInstance )
	{
		error( QtWarningMsg, "API already initialized (use reInit()!)", "initAPI()" );
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
			mDeviceInstance = Device::create(this);
			connect( mDeviceAPI, SIGNAL(newDeviceInfo(QString,QString)), mDeviceInstance, SLOT(setInfo(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newHardwareInterface(QString,QString)), mDeviceInstance, SLOT(addHardwareInterface(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newStateVariable(QHash<QString,QString>)), mStateManager, SLOT(registerStateVariable(QHash<QString,QString>)) );
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
			mDeviceInstance = Device::create(this);
			connect( mDeviceAPI, SIGNAL(newDeviceInfo(QString,QString)), mDeviceInstance, SLOT(setInfo(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newHardwareInterface(QString,QString)), mDeviceInstance, SLOT(addHardwareInterface(QString,QString)) );
			connect( mDeviceAPI, SIGNAL(newStateVariable(QHash<QString,QString>)), mStateManager, SLOT(registerStateVariable(QHash<QString,QString>)) );
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

void DeviceAPI::handleDeviceCommand( DeviceCommand *cmd )
{
	if( mEmitAllCmd )
	{
		emit commandReceived( cmd );
		return;
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
			else
				{ emit commandReceived( cmd ); }
		}
	}
	else
	{
		if( cmd->getType() == deviceCmdGet )
		{
			DeviceCommand *setCmd = DeviceCommand::build( deviceCmdSet, mStateManager->getVar( cmd->getHwInterface(), cmd->getVariable() ) );
			if( !mDeviceLink->sendCommand( setCmd ) )
				{ error( QtWarningMsg, "Failed to reply to device get", "handleDeviceCommand()" ); }
		}
		else if( cmd->getType() == deviceCmdSet )
		{
			DeviceStateVariable *var = mStateManager->getVar( cmd->getHwInterface(), cmd->getVariable() );
			if( var )
				{ var->setFromDevice( cmd->getArg() ); }
			else
				{ error( QtWarningMsg, QString("Failed to set variable from device, no such variable (hwI: %1, name: %2)").arg(cmd->getHwInterface(),cmd->getVariable()), "handleDeviceCommand()" ); }
		}
		else
			{ error( QtWarningMsg, "Got an undefined command?! Weird...", "handleDeviceCommand()" ); }
		cmd->deleteLater();
	}
}

void DeviceAPI::handleStateVariableUpdateRequest(DeviceStateVariable *stateVar)
{
	if( !mDeviceLink->sendCommand( DeviceCommand::build( deviceCmdGet, stateVar ) ) )
		{ error( QtWarningMsg, QString("Failed to update stateVar: %1").arg(stateVar->getName()), "handleStateVariableUpdateRequest()" ); }
}
