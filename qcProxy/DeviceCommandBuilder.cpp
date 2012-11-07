#include "DeviceCommandBuilder.h"
#include "Device.h"
#include "ProxySettingsManager.h"

using namespace QtuC;

DeviceCommandBuilder::DeviceCommandBuilder() :
	ErrorHandlerBase(),
	DeviceCommandBase(),
	mSeparator(' ')
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar();
}

DeviceCommandBuilder::DeviceCommandBuilder( const QString &commandString ) :
	ErrorHandlerBase(),
	DeviceCommandBase(),
	mSeparator(' ')
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

DeviceCommandBuilder::DeviceCommandBuilder(const DeviceCommandBase &cmdBase) :
	DeviceCommandBase(cmdBase),
	mSeparator(' ')
{
	mSeparator = ProxySettingsManager::instance()->value( "device/commandSeparator" ).toChar();
}

DeviceCommandBuilder *DeviceCommandBuilder::fromString( const QString &commandString )
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

	DeviceCommandBuilder *cmd = new DeviceCommandBuilder( commandString );

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

bool DeviceCommandBuilder::setInterface(const QString &hwi)
{
	if( !Device::isValidHwInterface(hwi) )
	{
		error( QtWarningMsg, QString("Try to set command from string with invalid hradware interface '%1', command ignored.").arg(hwi), "setInterface()" );
		return false;
	}
	else
	{
		mHwInterface = hwi;
		return true;
	}
}

const QString DeviceCommandBuilder::getCommandString() const
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

bool DeviceCommandBuilder::setArgumentString(const QString &argStr)
{
	QStringList rawArgList = argStr.split(mSeparator);
	QStringList argList;
	QString escapedArgCache;

	for( int i=0; i < rawArgList.size(); ++i )
	{
		if( rawArgList.at(i).startsWith(QChar('"')) )	// start an escaped argument
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

const QString DeviceCommandBuilder::getArgumentString() const
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
