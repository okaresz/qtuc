#ifndef DEVICECOMMAND_H
#define DEVICECOMMAND_H

#include <QObject>
#include <QString>
#include <DeviceCommandBase.h>

namespace QtuC
{

/** DeviceCommand class.
 *	Represents a device command. Use to communicate with the device. Inherits DeviceCommandBase.*/
class DeviceCommand : public DeviceCommandBase
{
	Q_OBJECT
public:
	/**	Constructor: create an invalid deviceCommand object.*/
	DeviceCommand();

	/** Create a DeviceCommand from a DeviceCommandBase object.
	 *	@param cmdBase A DeviceCommandBase object,*/
	DeviceCommand( const DeviceCommandBase &cmdBase );

	/** Create a DeviceCommand from a DeviceCommandBase object pointer.
	 *	@param cmdBase Pointer to a DeviceCommandBase object,?*/
	DeviceCommand( const DeviceCommandBase *cmdBase );

	/** Get the command string (as the device understands it).
	 *	@return The command as a QString, or an empty string if invalid.*/
	const QString getCommandString() const;

	/** Parse th passed command string, and create a DeviceCOmmand instance from it.
	 *	@param commandString The new command string.
	 *	@return The new DeviceCommand instance on success, 0 otherwise.*/
	static DeviceCommand *fromString( const QString &commandString );

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

	/** get arguments string from the command arguments.
	  *	Join arguments with command separator and generates a device-compatible string.
	  *	@return The device-compatible arguments string.*/
	const QString getArgumentString() const;

	QChar mSeparator;		///< Command delimiter. Used to separate command words from each other.
};

}	//QtuC::
#endif //DEVICECOMMAND_H

