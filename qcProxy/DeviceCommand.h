#ifndef DEVICECOMMAND_H
#define DEVICECOMMAND_H

#include "DeviceCommandBase.h"
#include "ErrorHandlerBase.h"
#include "DeviceStateProxyVariable.h"

namespace QtuC
{

/** DeviceCommandBuilder class.
 *	Used to build and parsed device command strings sent to/from the device.*/
class DeviceCommand : public ErrorHandlerBase, public DeviceCommandBase
{
	Q_OBJECT
public:
	/**	Constructor: create an invalid deviceCommand object.*/
	DeviceCommand();

	/** Create a DeviceCommandBuilder from a DeviceCommandBase object.
	 *	@param cmdBase A DeviceCommandBase object.*/
	DeviceCommand( const DeviceCommandBase &cmdBase );

	/** Get the command string (as the device understands it).
	 *	@return The command as a QString, or an empty string if invalid.*/
	const QString getCommandString() const;

	/** Parse th passed command string, and create a DeviceCOmmand instance from it.
	 *	@param commandString The new command string.
	 *	@return The new DeviceCommand instance on success, 0 otherwise.*/
	static DeviceCommand *fromString( const QString &commandString );

	/** Build a command from/for a device variable.
	  *	Build a command from the passed type, and the name and raw (device-side) value of the passed device variable.
	  *	@param cmdType Type of the command. Can be set or get, any other value will trigger an error.
	  *	@param stateVar Pointer to the device variable.
	  *	@return A command object, or 0 on failure.*/
	static DeviceCommand* fromVariable( deviceCommandType_t cmdType, const DeviceStateProxyVariable* stateVar );

	/// Inherited from DeviceCommandBase
	bool setInterface( const QString & hwi );

	/** Parse argument string and update the command object with the new argument data.
	  *	@return True on successful parsing, false otherwise.*/
	bool setArgumentString( const QString &argStr );

private:

	/**	Create a deviceCommand object from a command string.
	  * Private c'tor, use fromString() instead.
	 *	@param	commandString The valid command string.*/
	DeviceCommand( const QString &commandString );

	/** Apply a device variable to the command.
	 *	This will set the command variable and raw value from a DeviceStateVariable instance.
	 *	@param stateVar Pointer to a DeviceStateVariable instance.*/
	bool applyVariable( const DeviceStateProxyVariable *stateVar );

	/** get arguments string from the command arguments.
	  *	Join arguments with command separator and generates a device-compatible string.
	  *	@return The device-compatible arguments string.*/
	const QString getArgumentString() const;

	QChar mSeparator;		///< Command delimiter. Used to separate command words from each other.
};

}	//QtuC::

#endif //DEVICECOMMAND_H

