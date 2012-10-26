#ifndef DEVICEAPI_H
#define DEVICEAPI_H

#include <QObject>
#include "Device.h"
#include "DeviceStateManager.h"
#include "DeviceConnectionManagerBase.h"
#include "DeviceAPIFileHandler.h"

namespace QtuC
{

class DeviceStateVariable;

/** DeviceAPI class.
 * Acts as an interface to the various device variables, functions, states, communication, etc...*/
class DeviceAPI : public ErrorHandlerBase
{
	Q_OBJECT
public:
	/** Constructor.
	 *	@param apiDefString API definition string.*/
	DeviceAPI( QObject *parent = 0 );

	/** Call a device function.
	 *	@param hwInterface The hardvare interface.
	 *	@param function Name of the function.
	 *	@param arg Argument(s) for the function.
	 *	@return Returns true if function is successfully called, false otherwise.*/
	bool call( const QString &hwInterface, const QString &function, const QString &arg );

	/** Get a device state variable from the stateMeneger.
	  *	Please note, this function returns immediately with a DeviceStateVariable object, which may be outdated.
	  *	@todo How to request from device?
	  *	@param hwInterface Name of the hardware interface
	  *	@param varName name of the variable.
	  *	@return Pointer to a DeviceStateVariable object.*/
	const DeviceStateVariable* get( const QString &hwInterface, const QString &varName );


	/** Send a get command to the device to update this variable.
	 *	Use the valueUpdated() signal of the variable to know when the value has been updated.
	 *	This function returns immediately.
	 *	@param hwInterface The hardware interface.
	 *	@param varName Name of the variable.
	 *	@todo CALLBACK?
	 *	@return True if update sent successfully, false otherwise.*/
	bool update( const QString &hwInterface, const QString &varName );


	/** Set a new value for a state variable, asynchronously.
	 *	If device uses positive acknowledgement, this function only sends a set command to the device to which the device must respond with a similar set command.
	 *	The device response will trigger an update on the corresponding device state variable.
	 *	If device doesn't use positive ack, this function calls DeviceStateVariable::setValue() which will trigger a set command to the device.
	 *	In this case, if the device fails to update the value, the value in the device and the deviceStateVariable in qcProxy will not be in sync.
	 *	@todo Review implementation....
	 *	@param hwInterface The hardvare interface.
	 *	@param varName Name of the variable.
	 *	@param newVal The new user-side value.
	 *	@return Returns if set is successful, false otherwise.*/
	bool set( const QString &hwInterface, const QString &varName, const QVariant &newVal );

	/** Initialize the deviceAPI.
	 *	This function can only be called if no API is set so far. If you want to update the API, use reInitAPI().
	 *	@param apiDefString The API definition string. If omitted, the API will be loaded from file (path can be set in settings).
	 *	@return True if API is valid and is successfully applied, false otherwise.*/
	bool initAPI( const QString &apiDefString = QString() );


	/** Re-initialize the deviceAPI.
	 *	This function can only be used to update the API. To initialize DevieAPI for the first time, use initAPI().
	 *	@param apiDefString The API definition string. If omitted, the API will be loaded from file (path can be set in settings).
	 *	@return True if API is valid and is successfully applied, false otherwise. If API change fails, the previous API (if any) remains active.*/
	bool reInitAPI( const QString &apiDefString = QString() );

signals:

	/** Emitted if a message is received from the device
	 *	@param msgType Type of the message.
	 *	@param msg The message.*/
	void message( deviceMessageType_t msgType, QString msg );

private:

	DeviceStateManager* mStateManager;		///< The DeviceStateManager instance. Handles the device variables
	DeviceConnectionManagerBase* mDeviceLink;	///< DeviceConnectionManagerBase instance. Handles the connection to the device.
	DeviceAPIFileHandler *mDeviceAPI;		///< DeviceAPIFileHandler intance. handles deviceAPI and device API file.
	Device* mDeviceInstance;
};

}	//QtuC::
#endif //DEVICEAPI_H

