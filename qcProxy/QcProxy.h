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
 *	The QcProxy class is the main coordinator between the device and the clients.
 *	See route() methods for the actual data flow between them.*/
class QcProxy : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** Create proxy object.*/
	QcProxy( QObject *parent = 0 );

	~QcProxy();

	/** Start proxy.
	  *	@return True on success, false otherwise.*/
	bool start();

	/** Set behaviour to pass through.
	  *	In pass through mode, proxy will immediately relay all received device commands to all clients (as a ClientCommandDevice),
	  *	without storing any value or stateVariable. In passThrough mode, proxy is stateless.
	  *	@param pass True to pass through commands, false to normal mode.*/
	void setPassThrough( bool pass );

public slots:

	/** Route and process client commands.
	 *	@param clientCommand The client command.*/
	bool route( ClientCommandBase *clientCommand );

	/** Route and process device commands.
	 *	@param deviceCommand The device command.*/
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

	/** Handle subscription feed request and send the feed to the client.
	  *	Called on every ClientSubscription::subscriptionTick() of every subscription.
	  *	@param subscription The subscription who sent the request.*/
	void sendSubscriptionFeed( ClientSubscription *subscription );

private:
	DeviceAPI *mDevice;		///< API object for the device.
	ConnectionServer *mConnectionServer;	///< Holds the instance of the connection server.
	ClientSubscriptionManager *mClientSubscriptionManager;	///< Holds the instance of the subscription manager.

	bool mPassThrough;	///< If true, proxy will immediately relay all device commands to all clients (as a ClientCommandDevice)

};

}	//QtuC::
#endif //QCPROXY_H

