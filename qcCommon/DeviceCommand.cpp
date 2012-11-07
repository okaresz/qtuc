#include "DeviceCommand.h"

using namespace QtuC;

DeviceCommand::DeviceCommand() :
	ErrorHandlerBase(),
	DeviceCommandBase()
{
}

DeviceCommand::DeviceCommand(const DeviceCommand &deviceCommand) :
	ErrorHandlerBase(),
	DeviceCommandBase(deviceCommand)
{
}

DeviceCommand *DeviceCommand::build( deviceCommandType_t cmdType, const DeviceStateVariable *stateVar )
{
	if( cmdType == deviceCmdCall )
	{
		error( QtWarningMsg, "Try to build a variable command with call type", "build(deviceCommandType_t,DeviceStateVariable)", "DeviceCommand" );
		return 0;
	}
	if( !stateVar->isValid() )
	{
		error( QtWarningMsg, "Try to build a variable command with invalid variable", "build(deviceCommandType_t,DeviceStateVariable)", "DeviceCommand" );
		return 0;
	}

	DeviceCommand *deviceCommand = new DeviceCommand();

	deviceCommand->setType( cmdType );
	deviceCommand->setInterface( stateVar->getHwInterface() );
	deviceCommand->setVariable( stateVar->getName() );
	deviceCommand->setArg( stateVar->getDeviceReadyString() );
	if( deviceCommand->isValid() )
		{ return deviceCommand; }
	else
	{
		error( QtWarningMsg, "Validity check failed after build()", "build(deviceCommandType_t,DeviceStateVariable)", "DeviceCommand" );
		return 0;
	}
}

bool DeviceCommand::applyVariable( DeviceStateVariable *stateVar )
{
	if( stateVar->isValid() )
	{
		mVariable = stateVar->getName();
		mArgs.clear();
		mArgs.append( stateVar->getDeviceReadyString() );
		return isValid();
	}
	else
	{
		error( QtWarningMsg, "Try to set invalid variable object for command", "applyVariable()" );
		return false;
	}
}
