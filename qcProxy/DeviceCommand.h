#ifndef DEVICECOMMAND_H
#define DEVICECOMMAND_H

#include "DeviceCommandBase.h"
#include "ErrorHandlerBase.h"
#include "DeviceStateProxyVariable.h"

namespace QtuC
{

/** DeviceCommand class.
 *	Used to build and parse device command strings sent to/from the device.
 *	To create a command:
 *	  * you can call the constructor and use the setters to set all necessary command parts
 *	  * use fromString() to parse an existing device command
 *	  * use fromVariable() to build a `get` or `set` command for a particular variable.
 *	Use getCommandString() to get the string representation of the command.*/
class DeviceCommand : public ErrorHandlerBase, public DeviceCommandBase
{
	Q_OBJECT
public:
	/**	Constructor: create an invalid, empty deviceCommand object.*/
	DeviceCommand();

	/** Create a DeviceCommand from a DeviceCommandBase object.
	 *	@param cmdBase A DeviceCommandBase object.*/
	DeviceCommand( const DeviceCommandBase &cmdBase );

	/** Get the command string (as the device understands it).
	 *	@return The command as a QString, or an empty string if invalid.*/
	const QString getCommandString() const;

	/** Parse the passed command string, and create a DeviceCommand instance from it.
	 *	@param commandString The command string.
	 *	@return The new DeviceCommand instance on success, 0 otherwise.*/
	static DeviceCommand *fromString( const QString &commandString );

	/** Build a command from/for a device variable.
	  *	Build a command from the passed type, and the name and raw (device-side) value of the passed device variable.
	  *	@param cmdType Type of the command. Can be set or get, any other value will trigger an error.
	  *	@param stateVar Pointer to the device variable.
	  *	@return A command object, or 0 on failure.*/
	static DeviceCommand* fromVariable( deviceCommandType_t cmdType, const DeviceStateProxyVariable* stateVar );

	/** @name Inherited from DeviceCommandBase.
	  *	@{*/
	bool setInterface( const QString & hwi );
	bool setVariable( const QString& cv );
	/// @}

	/** Parse argument string and update the command object with the new argument data.
	  *	@return True on successful parsing, false otherwise.*/
	bool setArgumentString( const QString &argStr );

	/** Set command part separator character.
	 *	The change takes place immediately.
	 *	@param sepChar The new separator.*/
	static void setSeparator( QChar const &sepChar )
		{ mSeparator = sepChar; }

	/** Get command part separator character.
	  *	@return The current separator.*/
	static QChar const getSeparator()
		{ return mSeparator; }

private:

	/**	Create a deviceCommand object from a command string.
	  * Private c'tor, use fromString() instead.
	 *	@param	commandString A valid command string.*/
	DeviceCommand( const QString &commandString );

	/** Apply a device variable to the command.
	 *	This will set the command variable and raw value from a DeviceStateVariable instance.
	 *	@param stateVar Pointer to a DeviceStateVariable instance.*/
	bool applyVariable( const DeviceStateProxyVariable *stateVar );

	/** Get argument string from the command arguments.
	  *	Join arguments with command separator and generate a device-compatible string.
	  *	@return The device-compatible argument string.*/
	const QString getArgumentString() const;

	static QChar mSeparator;		///< Command delimiter. Used to separate command words from each other.
};

}	//QtuC::

#endif //DEVICECOMMAND_H

