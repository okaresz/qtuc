#include "DeviceAPI.h"
#include "DeviceCommand.h"
#include "SerialDeviceConnector.h"
#include "DeviceAPIFileHandler.h"

using namespace QtuC;

DeviceAPI::DeviceAPI( QObject *parent ) : ErrorHandlerBase(parent)
{
	mStateManager = new DeviceStateManager(this);
	mDeviceLink = new SerialDeviceConnector(this);
	mDeviceAPI = new DeviceAPIFileHandler(this);
	mDeviceInstance = 0;
}

bool DeviceAPI::call( const QString &hwInterface, const QString &function, const QString &arg )
{
	DeviceCommand dCmd;
	dCmd.setType( deviceCmdCall );
	dCmd.setInterface( hwInterface );
	dCmd.setFunction( function );
	if( !mDeviceLink->sendCommand( dCmd ) )
	{
		error( QtWarningMsg, "Device function call failed", "call" );
		return false;
	}
	return true;
}

const QVariant DeviceAPI::get( const QString &hwInterface, const QString &varName )
{
	DeviceStateVariable *var = mStateManager->getVar( hwInterface, varName );
	if( !var )
	{
		ErrorHandlerBase::errorDetails_t errDet;
		errDet.insert( "hwInterface", hwInterface );
		errDet.insert( "varName", varName );
		error( QtWarningMsg, "No such variable", "get()", errDet );
	}
	return var;
}

bool DeviceAPI::update( const QString &hwInterface, const QString &varName )
{
	if( !mDeviceLink->sendCommand( DeviceCommand( DeviceCommandBase::build( deviceCmdGet, get( hwInterface, varName ) ) ) ) )
	{
		errorDetails_t errDet;
		errDet.insert( "hwInterface", hwInterface );
		errDet.insert( "varName", varName );
		error( QtWarningMsg, "Failed to send get command-", "update()", errDet );
		return false;
	}
	return true;
}

bool DeviceAPI::set( const QString &hwInterface, const QString &varName, const QVariant &newVal )
{
	if( Device::positiveAck() )
	{
		DeviceStateVariable tmpVar( *( get(hwInterface, varName) ) );
		tmpVar.setValue(newVal);
		if( !mDeviceLink->sendCommand( DeviceCommand( DeviceCommandBase::build( deviceCmdSet, tmpVar ) ) ) )
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
		DeviceStateVariable* var = get( hwInterface, varName );
		if( var )
			{ var->setValue(newVal); }
		else
		{
			errorDetails_t errDet;
			errDet.insert( "posAck", "false");
			errDet.insert( "hwInterface", hwInterface );
			errDet.insert( "varName", varName );
			errDet.insert( "varVal", newVal.toString() );
			error( QtWarningMsg, "Failed to send set command", "set()", errDet );
			return false;
		}
	}
	return true;
}

bool DeviceAPI::initAPI( const QString &apiDefString )
{
	// Connect nothing on first pass, only if API is successfully parsed
	if( !mDeviceAPI->load() )
	{
		error( QtCriticalMsg, "Failed to load deviceAPI", "initAPI()" );
		return false;
	}
	else
	{
		mDeviceInstance = Device::instance(this);
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
		debug( debugLevelInfo, "deviceAPI loaded, Device created", "initAPI()" );
		return true;
	}
}

bool DeviceAPI::reInitAPI( const QString &apiDefString )
{
	return false;
	// Stop everithing, or delete everything (timers, autoupdates, subscriptions....
	// reinit StateManager, stateVars
	// reparse API, file
	/// @todo Implement
	mDeviceInstance->setCreated(false);
	if( !initAPI(apiDefString) )
	{
		error( QtCriticalMsg, "Failed to initialize deviceAPI", "reInitAPI()" );
		return false;
	}
	return true;
}
