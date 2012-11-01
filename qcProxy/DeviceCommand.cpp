#include "DeviceCommand.h"
#include "Device.h"
#include "ProxySettingsManager.h"

using namespace QtuC;

DeviceCommand::DeviceCommand() : DeviceCommandBase(), mSeparator(' ')
{}

DeviceCommand::DeviceCommand( const QString &commandString ) : DeviceCommandBase(), mSeparator(' ')
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar();

	QStringList cmdExploded = QString(commandString).remove('\n').split( mSeparator, QString::SkipEmptyParts );
	mType = commandTypeFromString( cmdExploded.at(0) );
	mHwInterface = cmdExploded.at(1);
	mVariable = cmdExploded.at(2);

	if( cmdExploded.size() > 3 )
	{
		cmdExploded.removeFirst();
		cmdExploded.removeFirst();
		cmdExploded.removeFirst();
		setArgumentString( cmdExploded.join(mSeparator) );
	}
}

DeviceCommand *DeviceCommand::fromString( const QString &commandString )
{
	QStringList cmdExploded = QString(commandString).remove('\n').split( ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar(), QString::SkipEmptyParts );

	if( cmdExploded.isEmpty() )
	{
		ErrorHandlerBase::error( QtWarningMsg, "Try to set empty command string, command ignored.", "fromString()", "DeviceCommand" );
		return 0;
	}
	if( cmdExploded.size() < 3 )
	{
		error( QtWarningMsg, "Try to set command from string containing less than 3 parts, command ignored.", "fromString()", "DeviceCommand" );
		return 0;
	}

	if( !Device::isValidHwInterface( cmdExploded.at(1) ) )
	{
		error( QtWarningMsg, QString("Try to set command from string with invalid hradware interface '%1', command ignored.").arg(cmdExploded.at(1)), "fromString()", "DeviceCommand" );
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
		error( QtWarningMsg, QString("Try to set command from string with invalid hradware interface '%1', command ignored.").arg(hwi), "setInterface()" );
		return false;
	}
	else
		{ mHwInterface = hwi; }
}

DeviceCommand::DeviceCommand(const DeviceCommandBase &cmdBase) : DeviceCommandBase(cmdBase), mSeparator(' ')
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar();
}

DeviceCommand::DeviceCommand(const DeviceCommandBase *cmdBase) : DeviceCommandBase(cmdBase), mSeparator(' ')
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar();
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
		{ strCmd = QString( mSeparator + getArgumentString() ); }

	if( !strCmd.endsWith('\n') )
		{ strCmd.append('\n'); }

	return strCmd.toAscii();
}

bool DeviceCommand::setArgumentString(const QString &argStr)
{
	QStringList rawArgList = argStr.split(mSeparator);
	QStringList argList;
	QString escapedArgCache;

	for( int i=0; i < rawArgList.size(); ++i )
	{
		if( rawArgList.at(i).startsWith(QChar('"')) )	// start an escaped argument
		{
			escapedArgCache = rawArgList.at(0).mid(1);
		}
		else if( rawArgList.at(i).startsWith(QChar('"')) )	// end of an escaped argument
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
	return checkSetValid();
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
