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

QList<DeviceStateVariable*> StateManagerBase::getVarList(const QString &hardwareInterface)
{
	if( hardwareInterface.isEmpty() )
		{ return *mStateVars; }

	QList<DeviceStateVariable*> varList;
	for( int i = 0; i < mStateVars->size(); ++i )
	{
		if( mStateVars->at(i)->getHwInterface() == hardwareInterface )
			{ varList.append( mStateVars->at(i) ); }
	}
	return varList;
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
	DeviceStateVariable *newStateVar = DeviceStateVariable::init( params["hwInterface"], params["name"], params["userType"], params["deviceType"], params["access-mode"], params["toUserScript"], params["toDeviceScript"] );
	if( newStateVar )
	{
		//set autoupdate if necessary
		/// @todo Only if this is the proxy?
		if( params.contains("autoUpdate-device") )
		{
			bool ok;
			int autoUpdateInterval = params.value( "autoUpdate-device").toInt(&ok);
			if( ok )
			{
				if( autoUpdateInterval > 0 )
				{
					if( !newStateVar->startAutoUpdate( autoUpdateInterval ) ) ///< @todo: only start auto updates globally, if device is connected and they can be processed?
						{ error( QtWarningMsg, QString("Unable to start auto update for variable %1 in hwInterface %2").arg(params.value("name"),params.value("hwInterface")), "registerStateVariable(QHash<QString,QString>)" ); }

				}
			}
			else
			{
				errorDetails_t errDet;
				errDet.insert( "hwi", params.value("hwInterface") );
				errDet.insert( "name", params.value("name") );
				errDet.insert( "autoUpdateIntervalStr", params.value( "autoUpdate-device") );
				error( QtWarningMsg, "Invalid value for auto update interval", "registerStateVariable(QHash<QString,QString>)", errDet );
			}
		}

		/// @todo gui hint

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
