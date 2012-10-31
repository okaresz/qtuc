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
 *	Device info monostate class (pure static).
 *	Holds some globally reachable information about the device.
 *	Before using this class, you must call it's create() method.*/
class Device : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** Constructor.
	 *	This will clear all existing static device data! Use swap() to replace only the Device object, and leave the static params.*/
	Device( QObject *parent = 0 );

	/** Create the Device object.
	 *	This will clear all existing static device data! Use swap() to replace only the Device object, and leave the static params.
	 *	@param hwInterfaceList A QStringList of the valid hardware interfaces.
	 *	@param hwInterfaceInfoList Info strings for the hardware interfaces.
	 *	@param deviceName Name of the device. (ASCII, max 50 char)*/
	Device( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent = 0 );

	/** Swap the Device singleton for the passed Device object.
	  *	Think twice before you use this!
	  *	@param newDevice The new Device singleton.*/
	Device* swap( Device *newDevice );

	/** Create the Device singleton.
	 *	Can only be called once, after that, Device will be readonly.
	 *	This will clear all existing static device data! Use swap() to replace only the Device object, and leave the static params.
	 *	@return The new Device instance.*/
	static Device *create( QObject *parent );

	/** Create the Device singleton.
	 *	Can only be called once, after that, Device will be readonly.
	 *	This will clear all existing static device data! Use swap() to replace only the Device object, and leave the static params.
	 *	@param hwInterfaceList A QStringList of the valid hardware interfaces.
	 *	@param hwInterfaceInfoList Info strings for the hardware interfaces.
	 *	@param deviceName Name of the device. (ASCII, max 50 char).
	 *	@return The new Device instance.*/
	static Device *create( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent );

	/** Get Device instance.
	  *	You can only get the pointer, if the device object has been created with create(), and the initialization has not yet occured, so setCreated() hasn't been called yet.
	  *	@return Pointer to the Device singleton, or 0 if it is already initialized.*/
	static Device *instance( QObject *parent = 0 );

	/** Get if hardware interface exists and is valid.
	  *	@param hwInterfaceName Name of the hardware interface.
	  *	@return True if hardware interface exists and is valid, false otherwise.*/
	static bool isValidHwInterface( const QString& hwInterfaceName ) const;

	/** Get info about a hardware interface.
	  *	@param hwInterfaceName Name of the hardware interface.
	  *	@return The info param of the hardware interface.*/
	static const QString getHwInterfaceInfo( const QString& hwInterfaceName ) const;

	/** Get device information.
	  *	Returns valid value only after the device handshake has happened.
	  *	@param key Name of the information.
	  *	@return The requested device information.*/
	static const QString getInfo( const QString & key ) const;

	/** Get the device name.
	 *	Returns valid value only after the device handshake has happened.
	 *	@return The device name.*/
	static const QString getName() const;

	/** Get the device description.
	 *	Returns valid value only after the device handshake has happened.
	 *	@return The device description 8if exists, empty string otherwise.*/
	static const QString getDescription() const;

	/** Get the device platform.
	 *	Returns valid value only after the device handshake has happened.
	 *	@return The device platform if exists, empty string otherwise.*/
	static const QString getPlatform() const;

	/** Get the device project.
	 *	Returns valid value only after the device handshake has happened.
	 *	@return The device project if exists, empty string otherwise.*/
	static const QString getProject() const;

	/** Get device connection status. Will be true only after a successful handshake.
	 *	@return COnnection status: true if connected, false if not or an error happened.*/
	//static bool isConnected() const;

	/** Set whether the device uses positive acknowledge to verify received commands.
	  *	@return True if device uses poitive acknowledge, false if not.*/
	static bool positiveAck() const
		{ return mPositiveAck; }


	/** Mark device as created.
	  *	After this function call, the device singleton cannot be reached or modified, only the static getters will work.*/
	void setCreated( bool created = true )
		{ mCreated = created; }

	/** Clear Device.
	  *	Delete all info and data.
	  *	@warning Think before you use this...*/
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
	void addHardwareInterface( const QString &hwInterfaceName, const QString &hwInterfaceInfo );

	/** Add new device function.
	  *	@param hwInterface Hardware interface of the function.
	  *	@param name Name of the function.
	  *	@param args Optional function arguments.*/
	void addFunction( const QString &hwInterface, const QString &name, const QString &args = QString() );

	/** Set device information.
	  *	@param key name of the information.
	  *	@param value Value of information.*/
	void setInfo( const QString &key, const QString &value );

	/** Set the connection status of the device.
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

	/** Set device project.
	 *	@param project The new project for the device.*/
	void setProject( const QString &project );

	/** Set whether the device uses positive acknowledge to verify received commands.
	  *	@param posAck Set to true if device uses poitive acknowledge, false if not.*/
	void setPositiveAck( bool posAck );

private:

	static Device *mInstance;	///< Pointer to the Device singleton.
	bool mCreated = false;	///< After the Device is created, it cannot be modified.
	static QStringList mHardwareInterfaces;	///< List of the valid hardware interfaces.
	static QHash<QString,QString> mHardwareInterfaceInfo;	///< List of hardware interface informations.
	static QList<QStringList> mFunctions;		///< Device function list.
	static bool mPositiveAck;	///< Whether the device uses positive acknowledge to verify received commands.
	static QHash<QString,QString> mInfo;	///< Several device information, parsed from deviceAPI.
};

}	//QtuC::
#endif //DEVICE_H

