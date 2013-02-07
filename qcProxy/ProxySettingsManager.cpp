#include "ProxySettingsManager.h"

using namespace QtuC;

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
}

ProxySettingsManager* ProxySettingsManager::instance(QObject *parent)
{
	if( !instancePtr )
	{
		instancePtr = new ProxySettingsManager(parent);
	}
	return (ProxySettingsManager*)instancePtr;
}
