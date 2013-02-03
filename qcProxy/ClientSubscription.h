#ifndef CLIENTSUBSCRIPTION_H
#define CLIENTSUBSCRIPTION_H

#include "ClientConnectionManagerBase.h"

namespace QtuC
{

class DeviceStateVariableBase;

/** Class to represent a clientSubscription.
  *	To every subscribe command a ClientSubscription is created, unless it results in a subscription duplicate.
  *	(For example a single variable will not be subscribed if the whole hardware interface of that variable is already subscribed.)
  *	Use start() to start the periodic emitting of subscriptionTick signal.*/
class ClientSubscription : public ErrorHandlerBase
{
	Q_OBJECT

public:

	/** Create a subscription.
	  *	@param client The subscribed client.
	  *	@param interval Interval of the subscription feed in milliseconds. Must be more than mMinSubscriptionInterval.
	  *	@param hwInterface The subscribed hardware interface.
	  *	@param variable The subscribed variable.
	  *	@param parent A compulsory parent to the subscription.*/
	explicit ClientSubscription( ClientConnectionManagerBase *client, quint32 interval, const QString &hwInterface, const QString &variable, QObject *parent );

	~ClientSubscription();

	/** Start the subscribe feed.
	  *	The subscriptionTick() signal will be emitted at the specified intervals.
	  *	@return True if the subscription is valid and the feed can be started, false otherwise.*/
	bool start();

	/// Stop emitting subscriptionTick(), subscription feed will stop.
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
	bool includes( const DeviceStateVariableBase *variable ) const;

	/** Get subscription variable.
	  *	 @return The variable.*/
	const QString getVariable() const
		{ return mVariable; }

	/** Get subscription hardware interface.
	  *	 @return The hardware interface.*/
	const QString getHwInterface() const
		{ return mHwInterface; }

	/** Get the interval of the subscription.
	  *	@return The interval of the subscription in milliseconds.*/
	quint32 getInterval() const
		{ return mInterval; }

	/** Get the client who requested this subscription.
	  *	@return The client who requested this subscription.*/
	ClientConnectionManagerBase *getClient() const
		{ return mClient; }

	/** Whether this subscription is valid.
	*	@return True if all members are initialized and valid, false otherwise.*/
	bool isValid();

	/** Get minimum subscription interval.
	 *	@todo include in settings. There's no reason for a setter.
	 *	@return The minimum interval of a subscription in milliseconds.*/
	static quint32 getMinSubscriptionInterval()
		{ return mMinSubscriptionInterval; }

public slots:

signals:

	/// Emitted when the timer of this subscription is timed out.
	void subscriptionTick();

	/** Emitted when this subscriiption can be deleted.
	  * (For example the client disconnected)*/
	void destroyMe();

private:

	/// Re-implement QObject::timerEvent().
	void timerEvent( QTimerEvent *timerEvent );

	ClientConnectionManagerBase *mClient;	///< The client that requested this subscription.
	QString mVariable;			///< Subscription variable.
	QString mHwInterface;		///< Subscription hardware interface.
	quint32 mInterval;	///< Interval of the subscripiton, in milliseconds, 32bit unsigned integer.
	int mTimerId;	///< Internal id of the QObject timer in use.
	static quint32 mMinSubscriptionInterval;	///< Minimum interval of a client subscription in milliseconds.

};

}	//QtuC::
#endif // CLIENTSUBSCRIPTION_H
