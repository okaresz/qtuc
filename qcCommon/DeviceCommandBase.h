#ifndef DEVICECOMMANDBASE_H
#define DEVICECOMMANDBASE_H

#include <QObject>
#include <QStringList>
#include "DeviceStateVariable.h"
#include "ErrorHandlerBase.h"

namespace QtuC
{

/// Device command type typedef.
enum deviceCommandType_t
{
	deviceCmdUndefined,
	deviceCmdGet,
	deviceCmdSet,
	deviceCmdCall
};

class DeviceCommandBase : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** Create an empty, invalid command object.*/
	DeviceCommandBase();

	/** Copy constructor.*/
	DeviceCommandBase( const DeviceCommandBase &deviceCommand );

	/** Copy from a pointer.*/
	DeviceCommandBase( DeviceCommandBase* deviceCommand );

	/** Build a command from/for a device variable.
	  *	This c'tor builds a command from the passed type, de variable name and the raw (device-side) value of the passed device variable.
	  *	@param cmdType Type of the command. Can be set or get, any other value will trigger an error.
	  *	@param stateVar Pointer to the device variable.
	  *	@return A command object, or 0 on failure.*/
	static DeviceCommandBase* build( deviceCommandType_t cmdType, DeviceStateVariable* stateVar );

	/** Get whether the current command object makes a valid device command.
	  *	@return True if valid, false if not.*/
	bool isValid() const;

	/** Get command type.
	  *	@return Command type.*/
	deviceCommandType_t getType() const
		{ return mType; }

	/** Get the hardware interface of the command.
	  *	@return Hardware interface name.*/
	const QString getInterface() const
		{ return mHwInterface; }

	/** Get command variable.
	  *	@return Command variable.*/
	const QString getVariable() const
		{ return mVariable; }

	/** Get command function name.
	  *	This is an alias for getVariable().
	  *	@return function name.*/
	const QString getFunction() const
		{ return getVariable(); }

	/** Get command arguments.
	  *	For get/set command, this gives the value, for a call command, the function arguments.
	  *	@return Command arguments as a concatenated string, as in a valid device command.*/
	const QString getArg() const;

	/** Get command argument list.
	  *	For get/set command, this gives the value (one lement in the list), for a call command, the function arguments.
	  *	@return Command arguments as a QStringList.*/
	const QStringList getArgList() const
		{ return mArgs; }

	/** Set command type.
	 *	@param type The command type to set.*/
	void setType( deviceCommandType_t type );

	/** Set hardware interface.
	 *	You can only set a valid hardware interface.
	 *	@param hwi The hardware interface to set.
	 *	@return True on success, false otherwise.*/
	bool setInterface( const QString & hwi );

	/** Set command variable.
	 *	You can only set a valid variable name.
	 *	@param cv command variable to set.
	 *	@return True on success, false otherwise.*/
	void setVariable( const QString& cv );

	/** Apply a device variable to the command.
	 *	This will set the command variable and value from a DeviceStateVariable instance.
	 *	@param stateVar Pointer to a DeviceStateVariable instance.*/
	bool applyVariable( DeviceStateVariable* stateVar );

	/** Set command function.
	  *	This is an alias for setVariable().
	  *	@param fn Function name to set.
	 *	@return True on success, false otherwise.*/
	bool setFunction( const QString& fn )
		{ return applyVariable(fn); }

	/** Set command arguments.
	  *	For get/set command, this sets the value, for a call command, the function arguments.
	  *	@param argStr Argument string.
	 *	@return True on success, false otherwise.*/
	bool setArg( const QString& argStr );

private:

	/** Check the minimum number of arguments for the current command type.
	  *	@return True if arguments are valid, false otherwise.*/
	bool checkArgCount();

	QString mHwInterface;		///< Hardware interface name
	deviceCommandType_t mType;	///< Command type.
	QString mVariable;			///< Command variable.
	QStringList mArgs;			///< Command arguments
	bool mValid;				///< Validity of this command,
};

}	//QtuC::
#endif //DEVICECOMMANDBASE_H
