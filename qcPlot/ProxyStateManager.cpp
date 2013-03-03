#include "ProxyStateManager.h"

using namespace qcPlot;

ProxyStateManager *ProxyStateManager::mInstance = 0;

ProxyStateManager::ProxyStateManager(QObject *parent) :
	StateManagerBase(parent)
{
}

ProxyStateManager *ProxyStateManager::instance( QObject *parent )
{
	if( mInstance )
	{
		return mInstance;
	}
	else
	{
		mInstance = new ProxyStateManager(parent);
		return mInstance;
	}
}

bool ProxyStateManager::registerNewStateVariable(QHash<QString, QString> params)
{
	if( params.contains("type") )
		{ params.insert( "userType", params["type"] ); }
	/// @todo Write init function of plotDataVar and use that
	DeviceStatePlotDataVariable *newStateVar = new DeviceStatePlotDataVariable( params["hwInterface"], params["name"], params["userType"], params["access-mode"] );
	if( newStateVar )
	{
		registerStateVariable( newStateVar );
		return true;
	}
	else
	{
		error( QtWarningMsg, "DeviceStatePlotDataVariable could not be created", "registerNewStateVariable(QHash<QString,QString>)" );
		return false;
	}
}
