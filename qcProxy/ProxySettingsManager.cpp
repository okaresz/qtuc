#include "ProxySettingsManager.h"
#include <QDebug>
#include <iostream>

using namespace QtuC;

QString ProxySettingsManager::mCmdArgNames[] = {
	QString("passthrough"),
	QString("verbose")
};

ProxySettingsManager::ProxySettingsManager(QObject *parent) :
	SettingsManagerBase(parent)
{
	if( !contains("apiFilePath") )
		{ setValue( "apiFilePath", "deviceAPI.xml" ); }

	// Device
	if( !contains("device/commandSeparator") )
		{ setValue( "device/commandSeparator", QChar(' ') ); }

	if( !contains("device/timeTicksPerMs") )
		{ setValue( "device/timeTicksPerMs", 1000.0 ); }

	// devicePort
	if( !contains("devicePort/portName") )
		{ setValue( "devicePort/portName", "/dev/ttyS1"); }

	if( !contains("devicePort/baudRate") )
		{ setValue( "devicePort/baudRate", 115200 ); }

	// serverInfo
	if( !contains("serverInfo/id") )
		{ setValue( "serverInfo/id", "qcProxy" ); }
	if( !contains("serverInfo/name") )
		{ setValue( "serverInfo/name", "qcProxy" ); }
	if( !contains("serverInfo/desc") )
		{ setValue( "serverInfo/desc", "Proxy application of QtuC framework." ); }
	if( !contains("serverInfo/author") )
		{ setValue( "serverInfo/author", "okaresz" ); }

	// serverSocket
	if( !contains("serverSocket/host") )
		{ setValue( "serverSocket/host", "localhost" ); }
	if( !contains("serverSocket/port") )
		{ setValue( "serverSocket/port", 24563 ); }
	if( !contains("serverSocket/heartBeatTimeout") )
		{ setValue( "serverSocket/heartBeatTimeout", 3 ); } // sec

	// Dummy device
	if( !contains("dummyDeviceSocket/host") )
		{ setValue( "dummyDeviceSocket/host", "localhost" ); }

	if( !contains("dummyDeviceSocket/port") )
		{ setValue( "dummyDeviceSocket/port", 8246 ); }

	// device log
	if( !contains("deviceLog/debugLogPath") )
		{ setValue( "deviceLog/debugLogPath", "deviceDebugMsgLog" ); }
	if( !contains("deviceLog/infoLogPath") )
		{ setValue( "deviceLog/infoLogPath", "deviceInfoMsgLog" ); }
	if( !contains("deviceLog/errorLogPath") )
		{ setValue( "deviceLog/errorLogPath", "deviceErrorMsgLog" ); }

	sync();

	// init command line params

	mCmdArgs.insert( cmdArgVerbose, QVariant((uint)0) );
	mCmdArgs.insert( cmdArgPassthrough, QVariant(false) );

	initCmdParser();
}

ProxySettingsManager* ProxySettingsManager::instance(QObject *parent)
{
	if( !instancePtr )
	{
		instancePtr = new ProxySettingsManager(parent);
	}
	return (ProxySettingsManager*)instancePtr;
}

void ProxySettingsManager::initCmdParser()
{
	static const QCommandLineConfigEntry conf[] = {
		{ QCommandLine::Switch, 'v', mCmdArgNames[cmdArgVerbose], "increase verbosity", QCommandLine::OptionalMultiple },
		{ QCommandLine::Switch, 'p', mCmdArgNames[cmdArgPassthrough], "Enable passThrough mode", QCommandLine::Optional },
		QCOMMANDLINE_CONFIG_ENTRY_END
	};
	mCmdParser->setConfig( conf );
}

void ProxySettingsManager::cmdSwitchFound(const QString &name)
{
	if( name == mCmdArgNames[cmdArgPassthrough] )
		{ mCmdArgs[cmdArgPassthrough] = true; }
	else if( name == mCmdArgNames[cmdArgVerbose] )
		{ mCmdArgs[cmdArgVerbose] = mCmdArgs.value(cmdArgVerbose).toUInt()+1; }
}

void ProxySettingsManager::cmdOptionFound(const QString &name, const QVariant &value)
{
	qDebug() << "Option:" << name << value;
}

void ProxySettingsManager::cmdParamFound(const QString &name, const QVariant &value)
{
	qDebug() << "Param:" << name << value;
}

void ProxySettingsManager::cmdParseError(const QString &error)
{
	std::cerr << qPrintable(error) << std::endl;
}

const QVariant ProxySettingsManager::getCmdArgValue(ProxySettingsManager::cmdArg_t arg)
{
	return mCmdArgs.value(arg);
}
