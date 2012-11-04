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
