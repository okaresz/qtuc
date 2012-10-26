#ifndef QCPROXY_H
#define QCPROXY_H

#include "ErrorHandlerBase.h"

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

private:
	DeviceAPI *mDevice;
	//GuiConnectionManager mGui;
	ConnectionServer *mConnectionServer;

private slots:
	/** Handle a newly connected client.
	 *	@param newClient The connected client object.*/
	void handleNewClient( ClientConnectionManagerBase *newClient );

};

}	//QtuC::
#endif //QCPROXY_H

