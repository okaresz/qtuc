#include "DeviceCommandBase.h"

using namespace QtuC;

DeviceCommandBase::DeviceCommandBase()
{
	mType = deviceCmdUndefined;
	mTimestamp = 0;
	mHasTimestamp = false;
}

DeviceCommandBase::DeviceCommandBase( const DeviceCommandBase &deviceCommand )
{
	mType = deviceCommand.getType();
	mTimestamp = deviceCommand.getTimestamp();
	mHasTimestamp = deviceCommand.hasTimestamp();
	mHwInterface = deviceCommand.getHwInterface();
	mVariable = deviceCommand.getVariable();
	mArgs = QStringList( deviceCommand.getArgList() );
}

bool DeviceCommandBase::isValid() const
{
	bool validity = true;
	validity = validity && mType != deviceCmdUndefined;
	validity = validity && !mHwInterface.isEmpty();
	validity = validity && !( mType == deviceCmdSet && (mArgs.isEmpty() || mVariable.isEmpty()) );
	validity = validity && !( mType == deviceCmdCall && mVariable.isEmpty() );
	return validity;
}

const QString DeviceCommandBase::getArg(int argIndex) const
{
	if( argIndex < mArgs.size() )
		{ return mArgs.at(argIndex); }
	else
		{ return QString(); }
}

bool DeviceCommandBase::setInterface( const QString &hwi )
{
	mHwInterface = hwi;
	return true;	// can't check, Device is in proxy
}

bool DeviceCommandBase::setVariable( const QString& cv )
{
	if( cv.isEmpty() )
		{ return false; }

	mVariable = cv;
	return true;
}

bool DeviceCommandBase::setArgList( const QStringList &argList )
{
	mArgs = QStringList( argList );
	return isValid();
}

bool DeviceCommandBase::setArg(const QString &arg, int index)
{
	if( ( index == 0 && mArgs.isEmpty() ) || index >= mArgs.size() )
	{
		mArgs.append( arg );
	}
	else
		{ mArgs[index] = arg; }
	return isValid();
}

bool DeviceCommandBase::appendArg(const QString &arg)
{
	mArgs.append( arg );
	return isValid();
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
