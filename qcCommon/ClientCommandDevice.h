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
	ClientCommandDevice( deviceCommandType_t type, const DeviceStateVariableBase *stateVariable = 0 );

	ClientCommandDevice( DeviceCommandBase *deviceCommand );

	// Inherited from ClientCommandBase

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	ClientCommandBase *exactClone();

	QDomElement getDomElement() const;

	bool isValid();

private:

	/** Do initializations which are common in all constructors.
	  *	@param type Type of the device command.*/
	void commonConstruct( deviceCommandType_t type );

};

}	//QtuC::
#endif //CLIENTCOMMANDDEVICE_H

