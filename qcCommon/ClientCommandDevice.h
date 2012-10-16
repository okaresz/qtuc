#ifndef CLIENTCOMMANDDEVICE_H
#define CLIENTCOMMANDDEVICE_H

#include "DeviceCommandBase.h"
#include "ClientCommandBase.h"

namespace QtuC
{

/**
 * Class ClientCommandDevice
 * Client Command used to wrap a deviceCommand.*/
class ClientCommandDevice : public DeviceCommandBase, public ClientCommandBase
{
public:
	ClientCommandDevice();

	ClientCommandDevice( DeviceCommandBase* deviceCommand );

};

}	//QtuC::
#endif //CLIENTCOMMANDDEVICE_H

