#ifndef DEVICESTATEMANAGER_H
#define DEVICESTATEMANAGER_H

#include "StateManagerBase.h"
#include "DeviceStateProxyVariable.h"

namespace QtuC
{

/** Class DeviceStateManager.
 *	Manages the state of the device through the DeviceStateVariables.*/
class DeviceStateManager : public StateManagerBase
{
	Q_OBJECT
public:
	DeviceStateManager( QObject *parent = 0 );

public slots:

	/// Re-implement to extend with proxyVariable-specific params.
	bool registerNewStateVariable( QHash<QString,QString> params );

signals:

	/** Emitted if a stateVariable requested an update.
	  *	@param stateVar The stateVariable who requested the update.*/
	void stateVariableUpdateRequest( DeviceStateProxyVariable *stateVar );

	/** Emitted when a state variable requested to send a set command with the new value.
	  *	@param stateVar The stateVariable who made the request.*/
	void stateVariableSendRequest( DeviceStateProxyVariable *stateVar );

private slots:

	/** Handle update request from a state variable.*/
	void onUpdateRequest();

	/** Rrequest to send a set command.*/
	void onSendRequest();
};

}	//QtuC::
#endif //DEVICESTATEMANAGER_H

