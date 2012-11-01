#include "ProxySettingsManager.h"

using namespace QtuC;

ProxySettingsManager::ProxySettingsManager(QObject *parent) :
	SettingsManagerBase(parent)
{
	// Device
	setValue( "device/commandSeparator", QChar(' ') );

	// devicePort
	setValue( "devicePort/portName", "/dev/ttyS1");
	setValue( "devicePort/baudRate", 115200 );

	// serverInfo
	setValue( "serverInfo/id", "qcProxy" );
	setValue( "serverInfo/name", "qcProxy" );
	setValue( "serverInfo/desc", "Proxy application of QtuC framework." );
	setValue( "serverInfo/author", "okaresz" );

	// serverSocket
	setValue( "serverSocket/host", "localhost" );
	setValue( "serverSocket/port", 24563 );
	setValue( "serverSocket/serverId", "qcProxy" );
	setValue( "serverSocket/heartBeatTimeout", 3 );	// sec
}
