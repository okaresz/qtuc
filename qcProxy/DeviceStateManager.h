#ifndef DEVICESTATEMANAGER_H
#define DEVICESTATEMANAGER_H

#include "StateManagerBase.h"

namespace QtuC
{

/** Class DeviceStateManager.
 * Manages the state of the device through the DeviceStateVariables.*/
class DeviceStateManager : public StateManagerBase
{
	Q_OBJECT
public:
	DeviceStateManager( QObject *parent = 0 );
};

}	//QtuC::
#endif //DEVICESTATEMANAGER_H

