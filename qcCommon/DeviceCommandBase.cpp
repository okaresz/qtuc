#include "DeviceCommandBase.h"

using namespace QtuC;

DeviceCommandBase::DeviceCommandBase() : ErrorHandlerBase()
{
	mType = deviceCmdUndefined;
	mValid = false;
}

DeviceCommandBase::DeviceCommandBase( const DeviceCommandBase &deviceCommand ) : ErrorHandlerBase()
{
	mType = deviceCommand.getType();
	mHwInterface = deviceCommand.getHwInterface();
	mVariable = deviceCommand.getVariable();
	mArgs = QStringList( deviceCommand.getArgList() );
	checkSetValid();
}

DeviceCommandBase::DeviceCommandBase( const DeviceCommandBase *deviceCommand ) : ErrorHandlerBase()
{
	mType = deviceCommand->getType();
	mHwInterface = deviceCommand->getHwInterface();
	mVariable = deviceCommand->getVariable();
	mArgs = QStringList( deviceCommand->getArgList() );
	checkSetValid();
}

DeviceCommandBase *DeviceCommandBase::build( deviceCommandType_t cmdType, const DeviceStateVariable *stateVar )
{
	if( cmdType == deviceCmdCall )
	{
		error( QtWarningMsg, "Try to build a variable command with call type", "build(deviceCommandType_t,DeviceStateVariable)", "DeviceCommandBase" );
		return 0;
	}
	if( !stateVar->isValid() )
	{
		error( QtWarningMsg, "Try to build a variable command with invalid variable", "build(deviceCommandType_t,DeviceStateVariable)", "DeviceCommandBase" );
		return 0;
	}

	DeviceCommandBase *deviceCommand = new DeviceCommandBase();

	deviceCommand->setType( cmdType );
	deviceCommand->setVariable( stateVar->getName() );
	deviceCommand->setArg( stateVar->getDeviceReadyString() );
	if( deviceCommand->checkSetValid() )
		{ return deviceCommand; }
	else
		{ error( QtWarningMsg, "Validity check failed after build()", "build(deviceCommandType_t,DeviceStateVariable)", "DeviceCommandBase" ); }
}

bool DeviceCommandBase::isValid() const
{
	return mValid;
}

const QString DeviceCommandBase::getArg(int argIndex) const
{
	if( argIndex < mArgs.size() )
		{ return mArgs.at(argIndex); }
	else
		{ return QString(); }
}

void DeviceCommandBase::setType( deviceCommandType_t type )
{
	mType = type;
	checkSetValid();
}

bool DeviceCommandBase::setInterface( const QString &hwi )
{
	mHwInterface = hwi;
}

void DeviceCommandBase::setVariable( const QString& cv )
{
	mVariable = cv;
	checkSetValid();
}

bool DeviceCommandBase::applyVariable( DeviceStateVariable *stateVar )
{
	if( stateVar->isValid() )
	{
		mVariable = stateVar->getName();
		mArgs.clear();
		mArgs.append( stateVar->getDeviceReadyString() );
		return checkSetValid();
	}
	else
	{
		error( QtWarningMsg, "Try to set invalid variable object for command", "applyVariable()" );
		return false;
	}
}

bool DeviceCommandBase::setArgList( const QStringList &argList )
{
	mArgs = QStringList( argList );
	return checkSetValid();
}

bool DeviceCommandBase::setArg(const QString &arg, int index)
{
	if( ( index == 0 && mArgs.isEmpty() ) || index >= mArgs.size() )
	{
		mArgs.append( arg );
	}
	else
		{ mArgs[index] = arg; }
}

const QString DeviceCommandBase::commandTypeToString( deviceCommandType_t cmdType )
{
	switch( cmdType )
	{
		case deviceCmdGet: return QString("get");
		case deviceCmdSet: return QString("set");
		case deviceCmdCall: return QString("call");
		default: return QString();
	}
}

deviceCommandType_t DeviceCommandBase::commandTypeFromString( const QString &typeStr )
{
	if( typeStr == "get" ) { return deviceCmdGet; }
	if( typeStr == "set" ) { return deviceCmdSet; }
	if( typeStr == "call" ) { return deviceCmdCall; }
	return deviceCmdUndefined;
}

bool DeviceCommandBase::checkSetValid()
{
	bool validity = true;
	validity = validity && mType != deviceCmdUndefined;
	validity = validity && !mHwInterface.isEmpty();
	validity = validity && !( mType == deviceCmdSet && mArgs.isEmpty() );
	mValid = validity;
	return mValid;
}

