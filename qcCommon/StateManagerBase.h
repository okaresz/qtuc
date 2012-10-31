#ifndef STATEMANAGERBASE_H
#define STATEMANAGERBASE_H

#include <QObject>
#include <QString>
#include <QHash>
#include "ErrorHandlerBase.h"

namespace QtuC
{

class DeviceStateVariable;

/** Class StateManagerBase
 *	Base for the state manager classes dealing with DeviceStateVariables.*/
class StateManagerBase : public ErrorHandlerBase
{
	Q_OBJECT

public:
	/** Constructor.
	  *	@param parent The usual optional parent object.*/
	StateManagerBase( QObject* parent = 0 );

	~ StateManagerBase();

	/** Get a pointer to  a specific device state variable.
	  *	@param hardwareInterface The hardware interface name containing the variable.
	  *	@param varName The name of the variable.
	  *	@return Pointer to the requested variable.*/
	DeviceStateVariable* getVar( const QString& hardwareInterface, const QString& varName );

	/** Register a deviceStateVariable.
	  *	Once a variable is registered, the stateManager handles it's signals, keep it updated , and so on...
	  *	@param stateVar The variable to manage.*/
	void registerStateVariable( DeviceStateVariable* stateVar );

signals:

	/** Emitted if a stateVariable requested an update.
	  *	@param stateVar The stateVariable who requested the update.*/
	void stateVariableUpdateRequest( DeviceStateVariable *stateVar );

	/** Emitted when a state variable requested a set command to device with a new raw value.
	  *	@param stateVar The stateVariable who made the request.
	  *	@param newRawVal The new Raw value to set on the device.*/
	void setVariableOnDeviceRequest( DeviceStateVariable *stateVar, QString newRawVal );

public slots:
	/** Create a deviceStateVariable from a QHash of params and register it if valid.
	  *	This slot is usually connected to DeviceAPIParser::newStateVariable() signal.
	  *	Tries to build a variable from the params passed. If the variable is created, it will be registered using registerStateVariable().
	  *	@param params The params to build a variable from.
	  *	@return True on success, false otherwise.*/
	bool registerStateVariable( QHash<QString,QString> params );

private slots:

	/** Handle update request from a state variable.*/
	void handleStateVariableUpdateRequest();

	/** Set a new value for variable on device.
	  *	Basically a request to send a set command to the device.
	  *	@param newRawVal The new raw value to set.*/
	void handleSetOnDevice( const QString &newRawVal );

private:
	QList<DeviceStateVariable*>* mStateVars;

};

}	//QtuC::
#endif //STATEMANAGERBASE_H

