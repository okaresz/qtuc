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
class DeviceAPI :QObject
{
	Q_OBJECT
public:
	/** Constructor.
	 *	@param apiDefString API definition string.*/
	DeviceAPI( const QString &apiDefString );

	/** Call a device function.
	 *	@param hwInterface The hardvare interface.
	 *	@param function Name of the function.
	 *	@param arg Argument(s) for the function.
	 *	@return Returns true if function is successfully called, false otherwise.*/
	bool call( const QString &hwInterface, const QString &function, const QString &arg );

	/** Get a device state variable from the stateMeneger.
	  *	@param hwInterface Name of the hardware interface
	  *	@param varName name of the variable.
	  *	@return Pointer to a DeviceStateVariable object.*/
	const DeviceStateVariable* get( const QString &hwInterface, const QString &varName );


	/** Send a get command to the device to update this variable.
	 *	Use the valueUpdated() signal of the variable to know when the update has finished.
	 *	This function returns immediately.
	 *	@param hwInterface The hardvare interface.
	 *	@param varName Name of the variable.
	 *	@todo CALLBACK?
	 *	@return True if update sent successfully, false otherwise.*/
	bool update( const QString &hwInterface, const QString &varName );


	/** Set a new value for a state variable, asynchronously.
	 *	This function is a shortcut for DeviceAPI::getVar(hwInterface,varName)->setvalue(newVal).
	 *	@param hwInterface The hardvare interface.
	 *	@param varName Name of the variable.
	 *	@return Returns if set is successful, false otherwise.*/
	bool set( const QString &hwInterface, const QString &varName, const QVariant &newVal );

	/** Initialize the deviceAPI from an API definition string.
	 *	This function can only be called if no API is set so far. If you want to update the API, use reInitAPI().
	 *	@param apiDefString The API definition string.
	 *	@return True if API is valid and is successfully applied, false otherwise.*/
	bool initAPI( const QString &apiDefString );


	/** Re-initialize the deviceAPI from an API definition string.
	 *	This function can only be used to update the API. To initialize DevieAPI for the first time, use initAPI().
	 *	@param apiDefString The API definition string.
	 *	@return True if API is valid and is successfully applied, false otherwise. If API change fails, the previous API (if any) remains active.*/
	bool reInitAPI( const QString &apiDefString );

signals:

	/** Emitted if a message is received from the device
	 *	@param msgType Type of the message.
	 *	@param msg The message.*/
	void message( deviceMessageType_t msgType, QString msg );

private:

	DeviceStateManager* mStateManager;		///< The DeviceStateManager instance. Handles the device variables
	DeviceConnectionManagerBase* mDeviceLink;	///< DeviceConnectionManagerBase instance. Handles the connection to the device.
	DeviceAPIFileHandler *mDeviceAPI;		///< DeviceAPIFileHandler intance. handles deviceAPI and device API file.
};

}	//QtuC::
#endif //DEVICEAPI_H

