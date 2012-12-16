#ifndef QCPROXY_H
#define QCPROXY_H

#include "ErrorHandlerBase.h"
#include "ClientCommandBase.h"
#include "DeviceCommand.h"
#include "Device.h"
#include "ClientSubscriptionManager.h"

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

	/** Start proxy.
	  *	@return True on success, false otherwise.*/
	bool start();

	/** Set behaviour to pass through.
	  *	In pass through mode, proxy will immediately relay all device commands to all clients (as a ClientCommandDevice).
	  *	@param pass True to pass through commands, falsi to normal mode.*/
	void setPassThrough( bool pass );

public slots:

	bool route( ClientCommandBase *clientCommand );
	bool route( DeviceCommand *deviceCommand );

	/** Handle incoming device message*
	  *	@param msgType Message type.
	  *	@param msg The message string.
	  * @return True on success, false otherwise.*/
	bool handleDeviceMessage( deviceMessageType_t msgType, QString msg );

private slots:
	/** Handle a newly connected client.
	 *	@param newClient The connected client object.*/
	void handleNewClient( ClientConnectionManagerBase *newClient );

	/** Handle subscription feed request and send the feed packet.
	  *	@param client The subscribed client.
	  *	@param hwInterface The subscribed hardware interface.
	  *	@param variable The subscribed variable.*/
	void sendSubscriptionFeed( ClientSubscription *subscription );

private:
	DeviceAPI *mDevice;		///< API object for the device.
	ConnectionServer *mConnectionServer;	///< Holds the instance of the connection server.
	ClientSubscriptionManager *mClientSubscriptionManager;

	bool mPassThrough;	///< If true, proxy will immediately relay all device commands to all clients (as a ClientCommandDevice)

};

}	//QtuC::
#endif //QCPROXY_H

