#ifndef QCPROXY_H
#define QCPROXY_H

#include "ErrorHandlerBase.h"
#include "ClientCommandBase.h"
#include "DeviceCommandBase.h"
#include "Device.h"

namespace QtuC
{

class ConnectionServer;
class DeviceAPI;
class ClientConnectionManagerBase;

/** QcProxy class.
 * The QcProxy class is the main coordinator between the device and the clients.*/
class QcProxy : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** C'tor.*/
	QcProxy( QObject *parent = 0 );

	~QcProxy();

public slots:

	bool route( ClientCommandBase *clientCommand );
	bool route( DeviceCommandBase *deviceCommand );

	/** Handle incoming device message*
	  *	@param msgType Message type.
	  *	@param msg The message string.
	  * @return True on success, false otherwise.*/
	bool handleDeviceMessage( deviceMessageType_t msgType, QString msg );

private slots:
	/** Handle a newly connected client.
	 *	@param newClient The connected client object.*/
	void handleNewClient( ClientConnectionManagerBase *newClient );

private:
	DeviceAPI *mDevice;
	//GuiConnectionManager mGui;
	ConnectionServer *mConnectionServer;

};

}	//QtuC::
#endif //QCPROXY_H

