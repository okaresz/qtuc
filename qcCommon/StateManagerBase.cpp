#include "StateManagerBase.h"
#include "DeviceStateVariableBase.h"

using namespace QtuC;

StateManagerBase::StateManagerBase( QObject *parent ) : ErrorHandlerBase(parent)
{
	mStateVars = new QList<DeviceStateVariableBase*>();
}

StateManagerBase::~StateManagerBase()
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

DeviceStateVariableBase* StateManagerBase::getVar( const QString& hardwareInterface, const QString& varName )
{
	for( int i = 0; i < mStateVars->size(); ++i )
	{
		if( mStateVars->at(i)->getHwInterface() == hardwareInterface && mStateVars->at(i)->getName() == varName )
			{ return mStateVars->value(i); }
	}
	return 0;
}

QList<DeviceStateVariableBase *> StateManagerBase::getVarList(const QString &hardwareInterface)
{
	if( hardwareInterface.isEmpty() )
		{ return *mStateVars; }

	QList<DeviceStateVariableBase*> varList;
	for( int i = 0; i < mStateVars->size(); ++i )
	{
		if( mStateVars->at(i)->getHwInterface() == hardwareInterface )
			{ varList.append( mStateVars->at(i) ); }
	}
	return varList;
}

void StateManagerBase::registerStateVariable( DeviceStateVariableBase *stateVar )
{
	stateVar->setParent(this);
	connect( stateVar, SIGNAL(updateMe()), this, SLOT(onUpdateRequest()) );
	connect( stateVar, SIGNAL(sendMe()), this, SLOT(onSendRequest()) );
	mStateVars->append(stateVar);
}

bool StateManagerBase::registerNewStateVariable(QHash<QString,QString> params)
{
	if( params.contains("type") )
		{ params.insert( "userType", params["type"] ); }
	DeviceStateVariableBase *newStateVar = DeviceStateVariableBase::init( params["hwInterface"], params["name"], params["userType"], params["access-mode"] );
	if( newStateVar )
	{
		registerStateVariable( newStateVar );
		return true;
	}
	else
	{
		error( QtWarningMsg, "DeviceStateVariableBase could not be created", "registerNewStateVariable(QHash<QString,QString>)" );
		return false;
	}
}

void StateManagerBase::onUpdateRequest()
{
	emit stateVariableUpdateRequest( (DeviceStateVariableBase*)sender() );
}

void StateManagerBase::onSendRequest()
{
	emit stateVariableSendRequest( (DeviceStateVariableBase*)sender() );
}
