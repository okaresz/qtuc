#ifndef DEVICECOMMAND_H
#define DEVICECOMMAND_H

#include "DeviceCommandBase.h"
#include "ErrorHandlerBase.h"

namespace QtuC
{

/** Represents a device command.*/
class DeviceCommand : public ErrorHandlerBase, public DeviceCommandBase
{
	Q_OBJECT
public:
	explicit DeviceCommand();

	/** Copy constructor.*/
	DeviceCommand( const DeviceCommand &deviceCommand );

	/** DeviceCommandBase copy constructor.*/
	DeviceCommand( const DeviceCommandBase &commandBase );

	/** Build a command from/for a device variable.
	  *	This c'tor builds a command from the passed type, de variable name and the raw (device-side) value of the passed device variable.
	  *	@param cmdType Type of the command. Can be set or get, any other value will trigger an error.
	  *	@param stateVar Pointer to the device variable.
	  *	@return A command object, or 0 on failure.*/
	static DeviceCommand* build( deviceCommandType_t cmdType, const DeviceStateVariable* stateVar );
	
	/** Apply a device variable to the command.
	 *	This will set the command variable and value from a DeviceStateVariable instance.
	 *	@param stateVar Pointer to a DeviceStateVariable instance.*/
	bool applyVariable( DeviceStateVariable* stateVar );
	
};

}	//QtuC::
#endif // DEVICECOMMAND_H
