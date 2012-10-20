#include "DeviceAPI.h"
#include "DeviceCommand.h"

using namespace QtuC;

DeviceAPI::DeviceAPI( const QString &apiDefString )
{

}

bool DeviceAPI::call( const QString &hwInterface, const QString &function, const QString &arg )
{
	DeviceCommand dCmd;
	dCmd.setType( deviceCmdCall );
	dCmd.setInterface( hwInterface );
	dCmd.setFunction( function );
	mDeviceLink->sendCommand();
}

const QVariant DeviceAPI::get( const QString &hwInterface, const QString &varName )
{

}

bool DeviceAPI::update( const QString &hwInterface, const QString &varName )
{

}

bool DeviceAPI::set( const QString &hwInterface, const QString &varName, const QVariant &newVal )
{

}

bool DeviceAPI::initAPI( const QString &apiDefString )
{

}

bool DeviceAPI::reInitAPI( const QString &apiDefString )
{

}
