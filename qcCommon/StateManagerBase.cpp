#include "StateManagerBase.h"
#include "DeviceStateVariable.h"

using namespace QtuC;

StateManagerBase::StateManagerBase( QObject *parent )
{
	mStateVars = new QList<DeviceStateVariable*>();
}

StateManagerBase::~ StateManagerBase()
{
	/// @todo Delete mStateVarlist and statevars in it, if they are not deleted yet by parent
}

DeviceStateVariable* StateManagerBase::getVar( const QString& hardwareInterface, const QString& varName )
{
	for( int i = 0; i < mStateVars->size(); ++i )
	{
		if( mStateVars->at(i)->getHwInterface() == hardwareInterface && mStateVars->at(i)->getName() == varName )
			{ return mStateVars->value(i); }
	}
	return 0;
}

void StateManagerBase::registerStateVariable( DeviceStateVariable *stateVar )
{
	stateVar->setParent(this);
	stateVar->connect( stateVar, SIGNAL(updateMe()), this, SLOT(handleStateVariableUpdateRequest()) );
	/// @todo connect the rest of the signals
	mStateVars->append(stateVar);
}

bool StateManagerBase::registerStateVariable(QHash<QString,QString> params)
{
	if( params.contains("type") )
	{
		params.insert( "userType", params["type"] );
		params.insert( "deviceType", params["type"] );
	}
	DeviceStateVariable *newStateVar = DeviceStateVariable::init( params["hwInterface"], params["name"], params["userType"], params["deviceType"], params["toUserScript"], params["toDeviceScript"] );
	if( newStateVar )
	{
		registerStateVariable( newStateVar );
		return true;
	}
	else
	{
		error( QtWarningMsg, "DeviceStateVariable could not be created", "registerStateVariable(QHash<QString,QString>)" );
		return false;
	}
}

void StateManagerBase::handleStateVariableUpdateRequest()
{

}
