#include "DeviceCommand.h"
#include "Device.h"
#include "ProxySettingsManager.h"

using namespace QtuC;

QChar DeviceCommand::mSeparator = ' ';

DeviceCommand::DeviceCommand() :
	ErrorHandlerBase(),
	DeviceCommandBase()
{}

DeviceCommand::DeviceCommand( const QString &commandString ) :
	ErrorHandlerBase(),
	DeviceCommandBase()
{
	QStringList cmdExploded = QString(commandString).remove('\n').split( mSeparator, QString::SkipEmptyParts );

	int partIndex = 0;

	mType = commandTypeFromString( cmdExploded.at(partIndex++) );

	// timestamp
	mHasTimestamp = false;
	if( cmdExploded.at(partIndex).at(0) == '@' )
	{
		bool ok;
		mTimestamp = cmdExploded[partIndex++].remove(0,1).toLongLong( &ok, 16 );
		if( !ok )
		{
			mTimestamp = 0;
			ErrorHandlerBase::error( QtWarningMsg, "Invalid timestamp, ignored.", "DeviceCommand()", "DeviceCommand" );
		}
		else
			{ mHasTimestamp = true; }
	}

	mHwInterface = cmdExploded.at(partIndex++);
	mVariable = cmdExploded.at(partIndex++);

	// If there's more, it must be argument
	if( cmdExploded.size() >= partIndex )
	{
		cmdExploded.erase( cmdExploded.begin(), cmdExploded.begin()+partIndex );
		setArgumentString( cmdExploded.join(mSeparator) );
	}
}

DeviceCommand::DeviceCommand(const DeviceCommandBase &cmdBase) :
	DeviceCommandBase(cmdBase)
{}

DeviceCommand *DeviceCommand::fromString( const QString &commandString )
{
	QStringList cmdExploded = QString(commandString).remove('\n').split( ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar(), QString::SkipEmptyParts );

	if( cmdExploded.isEmpty() )
	{
		ErrorHandlerBase::error( QtWarningMsg, "Try to set empty command string, ignored.", "fromString()", "DeviceCommand" );
		return 0;
	}
	if( cmdExploded.size() < 3 )
	{
		error( QtWarningMsg, "Try to set command from string containing less than 3 parts, ignored.", "fromString()", "DeviceCommand" );
		return 0;
	}

	int hwiIndex = 1;
	// timestamp?
	if( cmdExploded.at(1).at(0) == '@' )
		{ hwiIndex = 2; }

	if( !Device::isValidHwInterface( cmdExploded.at(hwiIndex) ) )
	{
		error( QtWarningMsg, QString("Try to set command from string with invalid hardware interface '%1', command ignored.").arg(cmdExploded.at(hwiIndex)), "fromString()", "DeviceCommand" );
		return 0;
	}

	DeviceCommand *cmd = new DeviceCommand( commandString );

	if( cmd->isValid() )
		{ return cmd; }
	else
	{
		errorDetails_t errDet;
		errDet.insert( "cmdString", commandString );
		error( QtWarningMsg, "DeviceCommand is invalid.", "fromString()", "DeviceCommand" );
		return 0;
	}
}

bool DeviceCommand::setInterface(const QString &hwi)
{
	if( !Device::isValidHwInterface(hwi) )
	{
		error( QtWarningMsg, QString("Try to set command from string with invalid (non-existent) hardware interface '%1'.").arg(hwi), "setInterface()" );
		return false;
	}
	else if( hwi.contains(mSeparator) )
	{
		error( QtWarningMsg, QString("Passed interface ('%1') contains the command separator character.").arg(hwi), "setInterface()" );
		return false;
	}
	else
	{
		mHwInterface = hwi;
		return true;
	}
}

bool DeviceCommand::setVariable( const QString &cv )
{
	if( cv.contains(mSeparator) )
	{
		error( QtWarningMsg, QString("Passed variable ('%1') contains the command separator character.").arg(cv), "setVariable()" );
		return false;
	}
	return DeviceCommandBase::setVariable(cv);
}

const QString DeviceCommand::getCommandString() const
{
	if( !isValid() )
	{
		error( QtWarningMsg, "Unable to build command string: command is invalid", "getCommandString()" );
		return QString();
	}

	QString strCmd;
	// Explicit cast for QStringBuilder to work. See http://blog.qt.digia.com/2011/06/13/string-concatenation-with-qstringbuilder/
	strCmd = QString( commandTypeToString(mType) + mSeparator + mHwInterface + mSeparator + mVariable );

	if( !mArgs.isEmpty() )
		{ strCmd += QString( mSeparator + getArgumentString() ); }

	if( !strCmd.endsWith('\n') )
		{ strCmd.append('\n'); }

	return strCmd.toAscii();
}

/// @todo optimize
bool DeviceCommand::setArgumentString(const QString &argStr)
{
	QStringList rawArgList = argStr.split(mSeparator);
	QStringList argList;
	QString escapedArgCache;

	for( int i=0; i < rawArgList.size(); ++i )
	{
		if( rawArgList.at(i).endsWith(QChar('"')) && rawArgList.at(i).startsWith(QChar('"')) )	// starts and ends with ", a full argument, just remove the quotes
		{
			argList.append( rawArgList.at(i).mid( 1, rawArgList.at(i).size()-2 ) );
		}
		else if( rawArgList.at(i).startsWith(QChar('"')) )	// start an escaped argument
		{
			escapedArgCache = rawArgList.at(i).mid(1);
		}
		else if( rawArgList.at(i).endsWith(QChar('"')) )	// end of an escaped argument
		{
			escapedArgCache.append(mSeparator).append( rawArgList.at(i).left(rawArgList.at(i).size()-1) );
			argList.append( escapedArgCache );
			escapedArgCache.clear();
		}
		else if( !escapedArgCache.isEmpty() )	// we are in an escaped argument
		{
			escapedArgCache.append(mSeparator).append( rawArgList.at(i) );
		}
		else	// normal, non-escaped argument
		{
			argList.append( rawArgList.at(i) );
		}
	}

	mArgs = QStringList(argList);
	return isValid();
}

const QString DeviceCommand::getArgumentString() const
{
	QStringList escapedArgStringList;
	for( int i=0; i < mArgs.size(); ++i )
	{
		if( mArgs.at(i).contains(mSeparator) )
			{ escapedArgStringList.append( QString( "\"" + mArgs.at(i) + "\"" ) ); }
		else
			{ escapedArgStringList.append( mArgs.at(i) ); }
	}
	return escapedArgStringList.join(mSeparator);
}

DeviceCommand *DeviceCommand::fromVariable( deviceCommandType_t cmdType, const DeviceStateProxyVariable *stateVar )
{
	if( cmdType == deviceCmdCall )
	{
		error( QtWarningMsg, "Try to build a variable command with call type", "fromVariable()", "DeviceCommand" );
		return 0;
	}
	if( !stateVar->isValid() )
	{
		error( QtWarningMsg, "Try to build a variable command with invalid variable", "fromVariable()", "DeviceCommand" );
		return 0;
	}

	DeviceCommand *deviceCommand = new DeviceCommand();

	deviceCommand->setType( cmdType );

	if( deviceCommand->applyVariable( stateVar ) )
		{ return deviceCommand; }
	else
	{
		error( QtWarningMsg, "Invalid command from variable", "fromVariable()", "DeviceCommand" );
		return 0;
	}
}

bool DeviceCommand::applyVariable( const DeviceStateProxyVariable *stateVar )
{
	if( stateVar->isValid() )
	{
		setInterface( stateVar->getHwInterface() );
		setVariable( stateVar->getName() );
		mArgs.clear();
		if( mType == deviceCmdSet )
			{ setArg( stateVar->getDeviceReadyString() ); }
		return isValid();
	}
	else
	{
		error( QtWarningMsg, "Try to set invalid variable object for command", "applyVariable()" );
		return false;
	}
}
