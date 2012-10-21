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
	//DeviceCommand();

	/**	Constructor: create a deviceCommand object from a command string.
	 *	@param	cmdStr The command string.*/
	DeviceCommand( const QString &cmdStr );

	/** Create a DeviceCommand from a DeviceCommandBase object.
	 *	@param cmdBase A DeviceCommandBase object,*/
	DeviceCommand( const DeviceCommandBase &cmdBase );

	/** Create a DeviceCommand from a DeviceCommandBase object pointer.
	 *	@param cmdBase Pointer to a DeviceCommandBase object,?*/
	DeviceCommand( const DeviceCommandBase *cmdBase );

	/** Get the command string (as the device understands it).
	 *	@return The command as a QString, or an empty string if invalid.*/
	const QString getCommandString();

	/** Change the command to the passed command string.
	 *	Parse command string and update the command object with the parsed data.
	 *	@param cmdStr The new command string.
	 *	@return True on success, false otherwise.*/
	bool setCommandString( const QString &cmdStr );

private:

	//QString mCmdString;		///< The string representation of the commmand.

	/** Parse argument string and update the command object with the new argument data.
	  *	@return True on successful parsing, false otherwise.*/
	bool setArgumentString( const QString &argStr );

	/** get arguments string from the command arguments.
	  *	Join arguments with command separator and generates a device-compatible string.
	  *	@return The device-compatible arguments string.*/
	const QString getArgumentString();

	QChar mSeparator = ' ';		///< Command delimiter. Used to separate command words from each other.
};

}	//QtuC::
#endif //DEVICECOMMAND_H

