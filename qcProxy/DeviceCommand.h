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

	/** Get the command string (as the device understands it).
	 *	@return The command as a QString, or an empty string if invalid.*/
	QString getCommandString();

	/** Change the command to the passed command string.
	 *	@param cmdStr The new command string.
	 *	@return True on success, false otherwise.*/
	bool setCommandString( const QString &cmdStr );

private:

	QString mCmdString;		///< The string representation of the commmand.

	/** Parse command string.
	  *	@return True on successful parsing, false otherwise.*/
	bool parseCommand();
};

}	//QtuC::
#endif //DEVICECOMMAND_H

