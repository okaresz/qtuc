#ifndef STATEMANAGERBASE_H
#define STATEMANAGERBASE_H

#include <QObject>
#include <QString>
#include <QHash>
#include "ErrorHandlerBase.h"

namespace QtuC
{

class DeviceStateVariableBase;

/** Class StateManagerBase
 *	Base for the state manager classes dealing with DeviceStateVariables.*/
class StateManagerBase : public ErrorHandlerBase
{
	Q_OBJECT

public:
	/** Constructor.
	  *	@param parent The usual optional parent object.*/
	StateManagerBase( QObject* parent = 0 );

	~StateManagerBase();

	/** Get a pointer to  a specific device state variable.
	  *	@param hardwareInterface The hardware interface name containing the variable.
	  *	@param varName The name of the variable.
	  *	@return Pointer to the requested variable.*/
	DeviceStateVariableBase* getVar( const QString& hardwareInterface, const QString& varName );

	/** Get all variables in a specified hadware interface, or all interfaces.
	  *	@param hardwareInterface Get all vars in this interface. If omitted or empty, all variables in all interfaces will be returned.
	  * @param List of variable pointers.*/
	QList<DeviceStateVariableBase*> getVarList( const QString &hardwareInterface = QString() );

	/** Register a deviceStateVariable.
	  *	Once a variable is registered, the stateManager handles it's signals, keep it updated , and so on...
	  *	@param stateVar The variable to manage.*/
	void registerStateVariable( DeviceStateVariableBase* stateVar );

signals:

	/** Emitted if a stateVariable requested an update.
	  *	@param stateVar The stateVariable who requested the update.*/
	void stateVariableUpdateRequest( DeviceStateVariableBase *stateVar );

	/** Emitted when a state variable requested to send a set command with the new value.
	  *	@param stateVar The stateVariable who made the request.*/
	void stateVariableSendRequest( DeviceStateVariableBase *stateVar );

public slots:
	/** Create a new deviceStateVariable from a QHash of params and register it if valid.
	  *	This slot is usually connected to DeviceAPIParser::newStateVariable() signal.
	  *	Tries to build a variable from the params passed. If the variable is created, it will be registered using registerStateVariable().
	  *	@param params The params to build a variable from.
	  *	@return True on success, false otherwise.*/
	virtual bool registerNewStateVariable( QHash<QString,QString> params );

private slots:

	/** Handle update request from a state variable.*/
	virtual void onUpdateRequest();

	/** Rrequest to send a set command.*/
	virtual void onSendRequest();

private:
	QList<DeviceStateVariableBase*>* mStateVars;

};

}	//QtuC::
#endif //STATEMANAGERBASE_H

