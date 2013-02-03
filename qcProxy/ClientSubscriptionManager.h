#ifndef CLIENTSUBSCRIPTIONMANAGER_H
#define CLIENTSUBSCRIPTIONMANAGER_H

#include "ErrorHandlerBase.h"
#include "ClientCommandDevice.h"
#include "ClientSubscription.h"

namespace QtuC
{

class ClientConnectionManagerBase;
class DeviceStateVariableBase;

/** Class to manage client subscriptions.
  *	Subscriptions are managed globally (with one ClientSubscriptionManager), and each subscription object stores a pointer to its own subscriber client.
  *	For detailed rules of subscription, see the [subscribe](@ref doc-clientProtocol-packets-control-subscribe) clientCommand.
  *	When a client subscribes, proxy only chekcs if there's a subscription exactly like the requested one.
  *	Whether there is a more specific one is tested for each variable when sending the subscription feed.*/
class ClientSubscriptionManager : public ErrorHandlerBase
{
	Q_OBJECT

public:
	/// C'tor.
	explicit ClientSubscriptionManager(QObject *parent = 0);

	/** Get whether a more specific subscription exists.
	 *	Get whether a more specific subscription (compared to the passed ) exists for the passed variable.
	 *	"More specific" is checked with ClientSubscription::includes().
	 *	Search only among the other subscriptions of the client of the passed subscription.
	 *	@param variable The variable to check.
	 *	@param subscription Search for subscriptions more specific than this.
	 *	@return True, if a more specific subscription is found, false otherwise.*/
	bool moreSpecificSubscriptionExists( const DeviceStateVariableBase* variable, const ClientSubscription *subscription ) const;

public slots:

	/** Subscribe client to the requested set of variables.
	 *	See [subscribe command](#doc-clientProtocol-packets-subscribe) for more info on how to use the arguments.
	 *	@param client The client to subscribe.
	 *	@param interval Interval of the subscription in milliseconds. Must be larger than ClientSubscription::minSubscriptionInterval.
	 *	@param hwInterface Hardware interface.
	 *	@param variable Variable name.*/
	bool subscribe( ClientConnectionManagerBase *client, quint32 interval, const QString &hwInterface, const QString &variable );

	/** Unsubscribe client from the requested set of variables.
	 *	See [unSubscribe command](#doc-clientProtocol-packets-unSubscribe) for more info on how to use the arguments.
	 *	@param client The client to subscribe.
	 *	@param hwInterface Hardware interface.
	 *	@param variable Variable name.*/
	void unSubscribe( ClientConnectionManagerBase *client, const QString &hwInterface, const QString &variable );

	/** Cancel a subscription.
	  *	Cancel subscription feed, and destroy subscription.
	  *	@param subscription The subscription to cancel.*/
	void destroySubscription( ClientSubscription *subscription = 0 );

private slots:

	/// Called when the timer of one of the subscriptions is timed out.
	void handleSubscriptionTick();

signals:

	/* Emitted when a subscription feed must be sent to the client.
	  *	@param client The client who requested the subscription.
	  *	@param cmdList Commands to be sent in the feed packet.*/
	//void sendSubscriptionFeed( ClientConnectionManagerBase *client, QList<ClientCommandDevice*> cmdList );

	/* Emitted when a subscription feed must be sent to the client.
	  *	@param client The client who requested the subscription.
	  *	@param cmd Command to be sent in the feed packet.*/
	//void sendSubscriptionFeed( ClientConnectionManagerBase *client, ClientCommandDevice *cmd );

	/** Emitted when the timer of this subscription is timed out.
	  *	@param subscription The sender subscription.*/
	void subscriptionFeedRequest( ClientSubscription *subscription );

private:

	QList<ClientSubscription*> mSubscriptions;	///< Holds the list of client subscriptions.

};

}	//QtuC::
#endif // CLIENTSUBSCRIPTIONMANAGER_H
