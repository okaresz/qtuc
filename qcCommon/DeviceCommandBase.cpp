#include "DeviceCommandBase.h"

using namespace QtuC;

DeviceCommandBase::DeviceCommandBase()
{
	mType = deviceCmdUndefined;
	mValid = false;
}

DeviceCommandBase::DeviceCommandBase( const DeviceCommandBase &deviceCommand )
{
	mType = deviceCommand.getType();
	mHwInterface = deviceCommand.getHwInterface();
	mVariable = deviceCommand.getVariable();
	mArgs = QStringList( deviceCommand.getArgList() );
	checkSetValid();
}

DeviceCommandBase::DeviceCommandBase( DeviceCommandBase* deviceCommand )
{
	mType = deviceCommand->getType();
	mHwInterface = deviceCommand->getHwInterface();
	mVariable = deviceCommand->getVariable();
	mArgs = QStringList( deviceCommand->getArgList() );
	checkSetValid();
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
	deviceCommand->setArgList( stateVar->getDeviceReadyString() );
	if( checkSetValid() )
		{ return deviceCommand; }
	else
		{ error( QtWarningMsg, "Validity check failed after build()", "build(deviceCommandType_t,DeviceStateVariable)" ); }
}

bool DeviceCommandBase::isValid() const
{
	return mValid;
}

const QString DeviceCommandBase::getArg(int argIndex)
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
	if( Device::isValidHwInterface(hwi) )
		{ mHwInterface = hwi; }
	else
	{ error( QtWarningMsg, QString("Try to set invalid hardware interface '%1' for command").arg(hwi), "setType()" ); }
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

void DeviceCommandBase::commandTypeFromString( const QString &typeStr )
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
	validity = validity && !mHwInterface.isEmpty() && Device::isValidHwInterface(mHwInterface);
	validity = validity && !( mType == deviceCmdSet && mArgs.isEmpty() );
	mValid = validity;
	return mValid;
}

