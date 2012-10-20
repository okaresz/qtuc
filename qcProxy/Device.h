#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include "DeviceCommandBase.h"

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
class Device
{
public:
	/** Create the Device static object.
	 *	Can only be called once, after that, Device will be readonly.
	 *	@param hwInterfaceList A QStringList of the valid hardware interfaces.
	 *	@param hwInterfaceInfoList Info strings for the hardware interfaces.
	 *	@param deviceName Name of the device. (ASCII, max 50 char)*/
	static void create( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName );

	/** Get if hardware interface exists and is valid.
	  *	@param hwInterfaceName Name of the hardware interface.
	  *	@return True if hardware interface exists and is valid, false otherwise.*/
	static bool isValidHwInterface( const QString& hwInterfaceName ) const;

	/** Get info about a hardware interface.
	  *	@param hwInterfaceName Name of the hardware interface.
	  *	@return The info param of the hardware interface.*/
	static const QString getHwInterfaceInfo( const QString& hwInterfaceName ) const;

	/** Get the device name.
	 *	Holds valid value only after the device handshake has happened.
	 *	@return The client name.*/
	static const QString getName() const;

	/** Get the device description.
	 *	Holds valid value only after the device handshake has happened.
	 *	@return The client description 8if exists, empty string otherwise.*/
	static const QString getDescription() const;

	/** Get device connection status. Will be true only after a successful handshake.
	 *	@return COnnection status: true if connected, false if not or an error happened.*/
	static bool isConnected() const;

	/** Set the connection status of the device.
	 *	@param connectedState The new connection status to set.*/
	static void setConnected( bool connectedState );


	/** Convert commandType to QString.
	 *	@param cmdType Command type to convert.
	 *	@return command type as a QString.*/
	static const QString commandTypeToString( deviceCommandType_t cmdType );


	/** Convert a QString to commandType.
	 *	@param cmdStr Command string to convert.
	 *	@return the command type (may be invalid).*/
	static deviceCommandType_t commandTypeFromString( const QString &cmdStr );


	/** Set device name.
	 *	Only ASCII, max 50 character.
	 *	@param deviceName The new name for the device.*/
	static void setName( const QString &deviceName );


	/** Set device description.
	 *	@param deviceDesc The new description for the device.*/
	static void setDescription( const QString & deviceDesc );

private:

	static  bool created;	///< After the Device monostate class is created, it cannot be modified.
	QStringList hardwareInterfaces;	///< List of the valid hardware interfaces.
	QHash< QString, QString > hardwareInterfaceInfo;	///< List of hardware interface informations
	QString name;	///< device name
	QString description;	///< Device description
	bool connected;	///< Whether the device is connected. Set to true by proxy after a successful handshake.
	QString platform;	///< platform string of device (read from deviceAPI)
	QString project;	///< project string of device (read from deviceAPI)
};

}	//QtuC::
#endif //DEVICE_H

