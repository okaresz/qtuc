#include "ProxySettingsManager.h"

using namespace QtuC;

ProxySettingsManager::ProxySettingsManager(QObject *parent) :
	SettingsManagerBase(parent)
{
	// Device
	setValue( "device/commandSeparator", QChar(' ') );

	// devicePort
	setValue( "devicePort/portName", "/dev/stty1");
	setValue( "devicePort/baudRate", 115200 );
}
