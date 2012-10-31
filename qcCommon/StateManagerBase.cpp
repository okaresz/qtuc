#include "StateManagerBase.h"
#include "DeviceStateVariable.h"

using namespace QtuC;

StateManagerBase::StateManagerBase( QObject *parent ) : ErrorHandlerBase(parent)
{
	mStateVars = new QList<DeviceStateVariable*>();
}

StateManagerBase::~ StateManagerBase()
{
	for( int i=0; i<mStateVars->size(); ++i )
	{
		if( mStateVars->at(i) )
		{
			mStateVars->value(i)->disconnect();
			delete mStateVars->value(i);
		}
	}
	mStateVars->clear();
	delete mStateVars;
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
	connect( stateVar, SIGNAL(updateMe()), this, SLOT(handleStateVariableUpdateRequest()) );
	connect( stateVar, SIGNAL(setOnDevice(QString)), this, SLOT(handleSetOnDevice(QString)) );
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
	emit stateVariableUpdateRequest( (DeviceStateVariable*)sender() );
}

void StateManagerBase::handleSetOnDevice(const QString &newRawVal)
{
	DeviceStateVariable *var = (DeviceStateVariable*)sender();
	emit setVariableOnDeviceRequest( var, newRawVal );
}
