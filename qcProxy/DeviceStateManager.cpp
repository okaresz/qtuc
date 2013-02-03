#include "DeviceStateManager.h"
#include "DeviceStateProxyVariable.h"

using namespace QtuC;

DeviceStateManager::DeviceStateManager( QObject *parent ) : StateManagerBase(parent)
{

}

bool DeviceStateManager::registerNewStateVariable( QHash<QString,QString> params )
{
	if( params.contains("type") )
	{
		params.insert( "userType", params["type"] );
		params.insert( "deviceType", params["type"] );
	}
	DeviceStateProxyVariable *newStateVar = DeviceStateProxyVariable::init( params["hwInterface"], params["name"], params["userType"], params["deviceType"], params["access-mode"], params["toUserScript"], params["toDeviceScript"] );
	if( newStateVar )
	{
		//set autoupdate if necessary
		if( params.contains("autoUpdate-device") )
		{
			bool ok;
			int autoUpdateInterval = params.value( "autoUpdate-device").toInt(&ok);
			if( ok )
			{
				if( autoUpdateInterval > 0 )
				{
					if( !newStateVar->startAutoUpdate( autoUpdateInterval ) ) ///< @todo: only start auto updates globally, if device is connected and they can be processed?
						{ error( QtWarningMsg, QString("Unable to start auto update for variable %1 in hwInterface %2").arg(params.value("name"),params.value("hwInterface")), "registerNewStateVariable(QHash<QString,QString>)" ); }

				}
			}
			else
			{
				errorDetails_t errDet;
				errDet.insert( "hwi", params.value("hwInterface") );
				errDet.insert( "name", params.value("name") );
				errDet.insert( "autoUpdateIntervalStr", params.value( "autoUpdate-device") );
				error( QtWarningMsg, "Invalid value for auto update interval", "registerNewStateVariable(QHash<QString,QString>)", errDet );
			}
		}

		/// @todo gui hint?

		registerStateVariable( (DeviceStateVariableBase*)newStateVar );
		return true;
	}
	else
	{
		error( QtWarningMsg, "DeviceStateVariable could not be created", "registerNewStateVariable(QHash<QString,QString>)" );
		return false;
	}
}

void DeviceStateManager::onUpdateRequest()
{
	emit stateVariableUpdateRequest( (DeviceStateProxyVariable*)sender() );
}

void DeviceStateManager::onSendRequest()
{
	emit stateVariableSendRequest( (DeviceStateProxyVariable*)sender() );
}
