#include "DeviceCommand.h"
#include "Device.h"
#include "ProxySettingsManager.h"

using namespace QtuC;

DeviceCommand::DeviceCommand( const QString &cmdStr ) : DeviceCommandBase()
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" );
}

DeviceCommand::DeviceCommand(const DeviceCommandBase &cmdBase) : DeviceCommandBase(cmdBase)
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" );
}

DeviceCommand::DeviceCommand(const DeviceCommandBase *cmdBase) : DeviceCommandBase(cmdBase)
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" );
}

const QString DeviceCommand::getCommandString()
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

bool DeviceCommand::setCommandString( const QString &strCmd )
{
	QString pCmd = strCmd.remove('\n');
	QStringList cmdExploded = pCmd.split( mSeparator, QString::SkipEmptyParts );

	if( cmdExploded.isEmpty() )
	{
		error( QtWarningMsg, "Try to set empty command string, command ignored.", "setCommandString()" );
		return false;
	}
	if( cmdExploded.size() < 3 )
	{
		error( QtWarningMsg, "Try to set command from string containing less than 3 parts, command ignored.", "setCommandString()" );
		return false;
	}

	if( Device::isValidHwInterface( cmdExploded.at(1) ) )
	{
		error( QtWarningMsg, QString("Try to set command from string with invalid hradware interface '%1', command ignored.").arg(cmdExploded.at(1)), "setCommandString()" );
		return false;
	}

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

	return checkValid();
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
	return checkValid();
}

const QString DeviceCommand::getArgumentString()
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
