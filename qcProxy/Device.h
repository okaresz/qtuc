#ifndef DEVICE_H
#define DEVICE_H

#include "ErrorHandlerBase.h"
#include <QString>
#include "DeviceCommandBase.h"
#include <QHash>

namespace QtuC
{

enum deviceMessageType_t
{
	deviceMsgUndefined,
	deviceMsgInfo,
	deviceMsgDebug,
	deviceMsgError
};

/** Device class.
 *	Device info class, storing the Device parameters as static members, and contains a singleton object which can be made immutable with setCreated(). <br>
 *	Before using this class, you must call it's create() method.
 *	The device should not be modified after it's been created and all the parameters were set. <br>
 *	@par How to use
 *	First create the Device object with create(), then set all necessary params (you can use the slots of the Device singleton), and finally call setCreated().
 *	After calling setCreated(), the Device singleton object cannot be reached, (unless a its pointer was saved previously), only the static getters are usable, or swap().
 *	With swap() you can set the Device singleton to a new object (without changing the static members), which is still mutable, until setCreated() is called.*/
class Device : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** Create an empty Device singleton.
	 *	Can only be called if no instance exists yet. Use clear() to delete the current.
	 *	@return The new Device instance.*/
	static Device *create( QObject *parent );

	/** Create the Device singleton.
	 *	Can only be called if no instance exists yet. Use clear() to delete the current.
	 *	@param hwInterfaceList A QStringList of the valid hardware interfaces.
	 *	@param hwInterfaceInfoList Info strings for the hardware interfaces.
	 *	@param deviceName Name of the device. (ASCII, max 50 char).
	 *	@return The new Device instance.*/
	static Device *create( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent );

	/** Get Device instance.
	  *	You can only get the pointer, if setCreated() hasn't been called yet.
	  *	@return Pointer to the Device singleton, or 0 if it is already initialized.*/
	static Device *instance();

	/** Swap the Device singleton for the passed Device object.
	  *	Leave the static members, and swap the device singleton object only.
	  * The new Device object (and so all static member of the class) will be modifiable until setCreated() is called on it.
	  *	Think twice before you use this!
	  *	@param newDevice The new Device singleton.*/
	Device* swap( Device *newDevice );

	/** Get if hardware interface exists and is valid.
	  *	@param hwInterfaceName Name of the hardware interface.
	  *	@return True if hardware interface exists and is valid, false otherwise.*/
	static bool isValidHwInterface( const QString& hwInterfaceName );

	/** Get info about a hardware interface.
	  *	@param hwInterfaceName Name of the hardware interface.
	  *	@return The info param of the hardware interface.*/
	static const QString getHwInterfaceInfo( const QString& hwInterfaceName );

	/** Get device information.
	  *	Returns valid value only after the device handshake has happened. Before that, this returns the values in the deviceAPI.
	  *	@param key Name of the information.
	  *	@return The requested device information.*/
	static const QString getInfo( const QString & key );

	/** Get device information list.
	  *	Returns valid value only after the device handshake has happened. Before that, this returns the values in the deviceAPI.
	  *	@return The list of current device informations.*/
	static const QHash<QString,QString> getInfoList()
		{ return mInfo; }

	/** Get the device name.
	 *	Returns valid value only after the device handshake has happened. Before that, this returns the values in the deviceAPI.
	 *	@return The device name.*/
	static const QString getName();

	/** Get the device description.
	 *	Returns valid value only after the device handshake has happened. Before that, this returns the values in the deviceAPI.
	 *	@return The device description 8if exists, empty string otherwise.*/
	static const QString getDescription();

	/** Get the device platform.
	 *	Returns valid value only after the device handshake has happened. Before that, this returns the values in the deviceAPI.
	 *	@return The device platform if exists, empty string otherwise.*/
	static const QString getPlatform();

	/** Get the device project.
	 *	Returns valid value only after the device handshake has happened. Before that, this returns the values in the deviceAPI.
	 *	@return The device project if exists, empty string otherwise.*/
	static const QString getProject();

	/** Get device connection status. Will be true only after a successful handshake.
	 *	@return COnnection status: true if connected, false if not or an error happened.*/
	//static bool isConnected() const;

	/** Set whether the device uses positive acknowledge to verify received commands.
	  *	@return True if device uses poitive acknowledge, false if not.*/
	static bool positiveAck()
		{ return mPositiveAck; }

	/** Get if device instance was created.
	  *	If device is created, th device instance cannot be reached, only swapped with swap().
	  *	@return True if the device has already been created, false otherwise.*/
	bool isCreated()
		{ return mCreated; }

	/** Mark device as created.
	  *	After this function call, the device singleton cannot be reached or modified, only the static getters will work.*/
	void setCreated( bool created = true )
		{ mCreated = created; }

	/** Clear Device.
	  *	Delete all info and data, the static members and the singleton object as well..
	  *	@warning Think twice before you use this...*/
	void clear();

	/** Convert string to deviceMessageType_t.
	  *	@param msgTypeStr deviceMessageType string to convert.
	  *	@return deviceMessageType enum value.*/
	static deviceMessageType_t messageTypeFromString( const QString &msgTypeStr );

	/** Convert deviceMessageType_t to string.
	  *	@param msgType deviceMessageType enum value to convert.
	  *	@return deviceMessageType string.*/
	static const QString messageTypeToString( deviceMessageType_t msgType );

public slots:

	/** Add a hardware interface.
	  *	@param hwInterfaceName Name od the new hardware interface.*/
	void addHardwareInterface( const QString &hwInterfaceName );

	/** Add hardware interface with info (short description).
	  *	@param hwInterfaceName Name of the new hardware interface.
	  *	@param hwInterfaceInfo Info to add.*/
	void addHardwareInterfaceInfo( const QString &hwInterfaceName, const QString &hwInterfaceInfo );

	/// Alias for addHardwareInterfaceInfo.
	void addHardwareInterface( const QString &hwInterfaceName, const QString &hwInterfaceInfo )
		{ addHardwareInterfaceInfo( hwInterfaceName, hwInterfaceInfo ); }

	/** Add new device function.
	  *	@param hwInterface Hardware interface of the function.
	  *	@param name Name of the function.
	  *	@param args Optional function arguments.*/
	void addFunction( const QString &hwInterface, const QString &name, const QString &args = QString() );

	/** Set device information.
	  *	@param key name of the information.
	  *	@param value Value of information.*/
	void setInfo( const QString &key, const QString &value );

	/* Set the connection status of the device.
	 *	@param connectedState The new connection status to set.*/
	//void setConnected( bool connectedState );

	/** Set device name.
	 *	Only ASCII, max 50 character.
	 *	@param deviceName The new name for the device.*/
	void setName( const QString &deviceName );

	/** Set device description.
	 *	@param deviceDesc The new description for the device.*/
	void setDescription( const QString & deviceDesc );

	/** Set device platform.
	 *	@param platform The new platform for the device.*/
	void setPlatform( const QString &platform );

	/** Set device project.
	 *	@param project The new project for the device.*/
	void setProject( const QString &project );

	/** Set whether the device uses positive acknowledge to verify received commands.
	  *	@param posAck Set to true if device uses poitive acknowledge, false if not.*/
	void setPositiveAck( bool posAck );

private:

	/** Private constructor.
	 *	Use the static overloaded function create().
	 *	This will clear all existing static device data! Use swap() to replace only the Device object, and leave the static params.
	 *	The Device must have a QObject parent.*/
	Device( QObject *parent );

	/** Create the Device object.
	 *	Use the static overloaded function create().
	 *	This will clear all existing static device data! Use swap() to replace only the Device object, and leave the static params.
	 *	@param hwInterfaceList A QStringList of the valid hardware interfaces.
	 *	@param hwInterfaceInfoList Info strings for the hardware interfaces.
	 *	@param deviceName Name of the device. (ASCII, max 50 char)*/
	Device( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent = 0 );

	static Device *mInstance;	///< Pointer to the Device singleton.
	bool mCreated;	///< After the Device is created, it cannot be modified.
	static QStringList mHardwareInterfaces;	///< List of the valid hardware interfaces.
	static QStringList mHardwareInterfaceInfo;	///< List of hardware interface informations.
	static QList<QStringList> mFunctions;		///< Device function list.
	static bool mPositiveAck;	///< Whether the device uses positive acknowledge to verify received commands.
	static QHash<QString,QString> mInfo;	///< Several device information, parsed from deviceAPI.
};

}	//QtuC::
#endif //DEVICE_H

