#include "DeviceCommandBase.h"
#include "Device.h"

using namespace QtuC;

DeviceCommandBase::DeviceCommandBase()
{

}

DeviceCommandBase::DeviceCommandBase( DeviceCommandBase* deviceCommand )
{

}

DeviceCommandBase *DeviceCommandBase::build( deviceCommandType_t cmdType, DeviceStateVariable *stateVar )
{
	if( cmdType == deviceCmdCall )
	{
		error( QtWarningMsg, "Try to build a variable command with call type", "build(deviceCommandType_t,DeviceStateVariable)" );
		return 0;
	}
	if( !stateVar->isValid() )
	{
		error( QtWarningMsg, "Try to build a variable command with invalid variable", "build(deviceCommandType_t,DeviceStateVariable)" );
		return 0;
	}

	DeviceCommandBase *deviceCommand = new DeviceCommandBase();

	deviceCommand->setType( cmdType );
	deviceCommand->setVariable( stateVar->getName() );
	deviceCommand->setArg( stateVar->getDeviceReadyString() );
	return deviceCommand;
}

DeviceCommandBase::DeviceCommandBase( const DeviceCommandBase &deviceCommand )
{

}

bool DeviceCommandBase::isValid() const
{
	return mValid;
}

QString DeviceCommandBase::getArg() const
{
	return mArgs.join( " " );
}

void DeviceCommandBase::setType( deviceCommandType_t type )
{
	mType = type;
}

bool DeviceCommandBase::setInterface( const QString &hwi )
{
	if( Device::isValidHwInterface(hwi) )
		{ mHwInterface = hwi; }
	else
	{ error( QtWarningMsg, QString("Try to set invalid hardware interface '%1' for command").arg(hwi), "setType()" ); }
}

void DeviceCommandBase::setVariable( const QString& cv )
{
	mVariable = cv;
}

bool DeviceCommandBase::applyVariable( DeviceStateVariable *stateVar )
{
	if( stateVar->isValid() )
	{
		mVariable = stateVar->getName();
		mArgs.clear();
		mArgs.append( stateVar->getDeviceReadyString() );
	}
	else
	{ error( QtWarningMsg, "Try to set invalid variable object for command", "applyVariable()" ); }

}

bool DeviceCommandBase::setArg( const QString& argStr )
{
	/// @todo Implement
}

bool DeviceCommandBase::checkArgCount()
{
	mValid = !( mType == deviceCmdSet && mArgs.isEmpty() );
	return mValid;
}

