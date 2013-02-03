#ifndef CLIENTCOMMANDDEVICE_H
#define CLIENTCOMMANDDEVICE_H

#include "DeviceCommandBase.h"
#include "ClientCommandBase.h"

namespace QtuC
{

class DeviceStateVariableBase;

/** ClientCommandDevice class.
 *	Client Command used to represent a deviceCommand on the client side.*/
class ClientCommandDevice : public ClientCommandBase, public DeviceCommandBase
{
	Q_OBJECT
public:
	/** Create a ClientCommandDevice from a type and a stateVariable.
	  *	@param type Type of the deviceCommand: *get* or *set*. *call* is invalid here (not using a variable, it's meaningless in this case).
	  *	@param stateVariable Use this variable to fill the hwInterface and variable name, also value if command type is *set*.*/
	ClientCommandDevice( deviceCommandType_t type, const DeviceStateVariableBase *stateVariable = 0 );

	/** Creates a ClientCommandDevice based on a DeviceCommandBase.
	  *	@param deviceCommand The deviceCommand.*/
	ClientCommandDevice( DeviceCommandBase *deviceCommand );

	/**	@name Inherited from ClientCommandBase
	*	@{*/
	bool applyDomElement( const QDomElement &cmdElement );
	ClientCommandBase *clone();
	ClientCommandBase *exactClone();
	QDomElement getDomElement() const;
	bool isValid();
	/// @}

private:

	/** Do initializations which are common in all constructors.
	  *	@param type Type of the device command.*/
	void commonConstruct( deviceCommandType_t type );

};

}	//QtuC::
#endif //CLIENTCOMMANDDEVICE_H

