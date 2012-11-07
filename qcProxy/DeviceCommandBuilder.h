#ifndef DEVICECOMMANDBUILDER_H
#define DEVICECOMMANDBUILDER_H

#include "DeviceCommandBase.h"
#include "ErrorHandlerBase.h"

namespace QtuC
{

/** DeviceCommandBuilder class.
 *	Used to build and parsed device command strings sent to/from the device.*/
class DeviceCommandBuilder : public ErrorHandlerBase, public DeviceCommandBase
{
	Q_OBJECT
public:
	/**	Constructor: create an invalid deviceCommand object.*/
	DeviceCommandBuilder();

	/** Create a DeviceCommandBuilder from a DeviceCommandBase object.
	 *	@param cmdBase A DeviceCommandBase object.*/
	DeviceCommandBuilder( const DeviceCommandBase &cmdBase );

	/** Get the command string (as the device understands it).
	 *	@return The command as a QString, or an empty string if invalid.*/
	const QString getCommandString() const;

	/** Parse th passed command string, and create a DeviceCOmmand instance from it.
	 *	@param commandString The new command string.
	 *	@return The new DeviceCommand instance on success, 0 otherwise.*/
	static DeviceCommandBuilder *fromString( const QString &commandString );

	/// Inherited from DeviceCommandBase
	bool setInterface( const QString & hwi );

	/** Parse argument string and update the command object with the new argument data.
	  *	@return True on successful parsing, false otherwise.*/
	bool setArgumentString( const QString &argStr );

private:

	/**	Create a deviceCommand object from a command string.
	  * Private c'tor, use fromString() instead.
	 *	@param	commandString The valid command string.*/
	DeviceCommandBuilder( const QString &commandString );

	/** get arguments string from the command arguments.
	  *	Join arguments with command separator and generates a device-compatible string.
	  *	@return The device-compatible arguments string.*/
	const QString getArgumentString() const;

	QChar mSeparator;		///< Command delimiter. Used to separate command words from each other.
};

}	//QtuC::
#endif //DEVICECOMMANDBUILDER_H

