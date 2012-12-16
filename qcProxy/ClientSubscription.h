#ifndef CLIENTSUBSCRIPTION_H
#define CLIENTSUBSCRIPTION_H

#include "DeviceStateVariable.h"
#include "ClientConnectionManagerBase.h"

namespace QtuC
{

/** Class to represent a clientSubscription.
  *	To every subscribe command a ClientSubscription is created, unless it results in a subscription duplicate.
  *	(For example a single variable will not be subscribed if the whole hardware interface of that variable is already subscribed.)
  *	Use start() to start the periodic emitting of subscriptionTick signal.*/
class ClientSubscription : public ErrorHandlerBase
{
	Q_OBJECT

public:

	/** Create a subscription.
	  *	@param freq Frequency of the subscription feed.
	  *	@param client The subscribed client.
	  *	@param hwInterface The subscribed hardware interface.
	  *	@param variable The subscribed variable.*/
	explicit ClientSubscription( ClientConnectionManagerBase *client, unsigned int freq, const QString &hwInterface, const QString &variable, QObject *parent );

	~ClientSubscription();

	/** Start the subscribe feed.
	  *	The subscriptionTick() signal will be emitted at the specified intervals.
	  *	@return True if the subscription is valid and the feed can be started, false otherwise.*/
	bool start();

	/// Stop emitting subscriptionTick().
	void stop();

	/** Custom equality operator.
	  *	Two ClientSubscription is equal if both the hardware interface and the variable is the same. Frequency may be different.
	  *	@return True if the two ClientSubscriptions are equal, false if not.*/
	bool operator==( const ClientSubscription &otherSubscription ) const;

	/** Get whether this subscription includes the passed.
	  *	For example subscription B is included in A, if their hardware interfaces are the same,
	  * the variable of A is empty (all variable), variable of B is not empty (one specific variable). Then A already updates the variable of B.
	  *	@param other The other subscription.
	  *	@return True if this subscription includes or equals the passed.*/
	bool includes( const ClientSubscription &other ) const;

	/** Get whether the passed variable is included in the subscription.
	  *	A variable is *included* in a subscription, if:
	  *	  * the variable name and hardware interface name mathes the variable,
	  *	  * the hardware interface name matches (and the variable name is empty in the subscription)
	  *	  * both hardware interface name and variable name of the subscription is empty (every variable is included).
	  *	@param variable The variable to check.
	  *	@return True if the passed variable is included in the subscription.*/
	bool includes( const DeviceStateVariable *variable ) const;

	/** Get subscritpion variable.
	  *	 @return The variable.*/
	const QString getVariable() const
		{ return mVariable; }

	/** Get subscritpion hardware interface.
	  *	 @return The hardware interface.*/
	const QString getHwInterface() const
		{ return mHwInterface; }

	/** Get the frequency of the subscription.
	  *	@return The frequency of the subscription.*/
	unsigned int getFrequency() const
		{ return mFrequency; }

	/** Get the client who requested this subscription.
	  *	@return The client who requested this subscription.*/
	ClientConnectionManagerBase *getClient() const
		{ return mClient; }

	/** Whether this subscription is valid.
	*	@return True if all members are initialized and valid, false otherwise.*/
	bool isValid();

	/// Maximum frequency of a client subscription.
	static unsigned int maxSubscriptionFrequency;

public slots:

signals:

	/// Emitted when the timer of this subscription is timed out.
	void subscriptionTick();

	/** Emitted when this subscriiption can be deleted.
	  * (For example the client disconnected)*/
	void destroyMe();

private:

	void timerEvent( QTimerEvent *timerEvent );

	ClientConnectionManagerBase *mClient;	///< The client that requested this subscription.
	QString mVariable;		///< Subscription variable.
	QString mHwInterface;	///< Subscription hardware interface.
	unsigned int mFrequency;			///< Frequency of the subscripiton.
	int mTimerId;
};

}	//QtuC::
#endif // CLIENTSUBSCRIPTION_H
