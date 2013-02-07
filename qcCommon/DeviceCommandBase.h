#ifndef DEVICECOMMANDBASE_H
#define DEVICECOMMANDBASE_H

#include <QStringList>

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

/** Base class for representing a device command.
*	Stores the basic parts of a device command without any device specific information.*/
class DeviceCommandBase
{
public:

	/** Create an empty, invalid command object.*/
	DeviceCommandBase();

	/** Copy constructor.*/
	DeviceCommandBase( const DeviceCommandBase &deviceCommand );

	/** Get whether the current command object makes a valid device command.
	  *	This is a virtual function, you can reimplement it if necessary, but don't forget to call this also (DeviceCommandBase::isValid()).
	  *	@return True if valid, false if not.*/
	virtual bool isValid() const;

	/** Get command type.
	  *	@return Command type.*/
	deviceCommandType_t getType() const
		{ return mType; }

	/** Get command timestamp.
	 *	@return command timestamp*/
	quint64 getTimestamp() const
		{ return mTimestamp; }

	/** Get if there was a timestamp in the command.
	  *	@return True if there was a timestamp in the command, false otherwise.*/
	bool hasTimestamp() const
		{ return mHasTimestamp; }

	/** Get the hardware interface of the command.
	  *	@return Hardware interface name.*/
	const QString getHwInterface() const
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

	/** Get argument at index.
	  *	@param argIndex Index of argument. If omitted, return the one with index 0 (the first argument).
	  *	@return Command argument at index argIndex.*/
	const QString getArg( int argIndex = 0 ) const;

	/** Get command argument list.
	  *	For get/set command, this gives the value (one lement in the list), for a call command, the function arguments.
	  *	@return Command arguments as a QStringList.*/
	const QStringList getArgList() const
		{ return mArgs; }

	/** Get if command has argument(s)
	 *	@return True if the command has one or more argument, otherwise false.*/
	bool hasArg() const
		{ return !mArgs.isEmpty(); }

	/** Set command type.
	 *	@param type The command type to set.*/
	void setType( deviceCommandType_t type )
		{ mType = type; }

	/** Set timestamp.
	 *	@param timestamp New value for the timestamp.*/
	 void setTimestamp( quint64 const &timestamp )
		{ mTimestamp = timestamp; }

	/** Set hardware interface.
	 *	You can only set a valid hardware interface.
	 *	@param hwi The hardware interface to set.
	 *	@return True on success, false otherwise.*/
	virtual bool setInterface( const QString & hwi );

	/** Set command variable.
	 *	You can only set a valid variable name.
	 *	A valid name doesn't contain the command separator character, short and preferably consists ony ASCII characters.
	 *	@param cv command variable to set.
	 *	@return True on success, false otherwise.*/
	virtual bool setVariable( const QString& cv );

	/** Set command function.
	  *	This is an alias for setVariable().
	  *	@param fn Function name to set.
	 *	@return True on success, false otherwise.*/
	void setFunction( const QString& fn )
		{ setVariable(fn); }

	/** Set command arguments.
	  *	For get/set command, this sets the value, for a call command, the function arguments.
	  *	@param argList Argument(s) as a QStringList.
	 *	@return True on success, false otherwise.*/
	bool setArgList( const QStringList& argList );

	/** Set argument.
	  *	If argument already exist, overwrite, otherwise append. If index is greater than argument count, the argument is appended.
	  *	@param arg The argument value.
	  *	@param index The index of the argument to set. 0 if omitted.
	  *	@return True on success, false otherwise.*/
	bool setArg( const QString &arg, int index = 0 );

	/** Append an argument.
	  *	@param arg The argument value.
	  *	@return True on success, false otherwise.*/
	bool appendArg( const QString &arg );

	/** Convert commandType to QString.
	 *	@param cmdType Command type to convert.
	 *	@return command type as a QString.*/
	static const QString commandTypeToString( deviceCommandType_t cmdType );

	/** Convert a QString to commandType.
	 *	@param typeStr Type string to convert.
	 *	@return the command type (may be undefined).*/
	static deviceCommandType_t commandTypeFromString( const QString &typeStr );

protected:
	deviceCommandType_t mType;	///< Command type.

	bool mHasTimestamp;		///< Store if there was a timestamp in the command.

	/** Command timestamp.
	 *	The actual value and representation is device-dependent.
	 *	If there wasn't a timestamp in the command, mHasTimestamp is false and this is 0.*/
	quint64 mTimestamp;

	QString mHwInterface;		///< Hardware interface name
	QString mVariable;			///< Command variable.
	QStringList mArgs;			///< Command arguments
};

}	//QtuC::
#endif //DEVICECOMMANDBASE_H

