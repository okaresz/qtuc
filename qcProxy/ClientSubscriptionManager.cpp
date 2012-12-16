#include "ClientSubscriptionManager.h"

using namespace QtuC;

ClientSubscriptionManager::ClientSubscriptionManager(QObject *parent) :
	ErrorHandlerBase(parent)
{
}

bool ClientSubscriptionManager::subscribe(ClientConnectionManagerBase *client, unsigned int frequency, const QString &hwInterface, const QString &variable)
{
	ClientSubscription *subscription = new ClientSubscription( client, frequency, hwInterface, variable, this );

	bool found = false;
	for( int i=0; i<mSubscriptions.size(); ++i )
	{
		if( *(mSubscriptions.at(i)) == *subscription )
		{
			found = true;
			break;
		}
	}

	if( !found )
	{
		mSubscriptions.append( subscription );
		connect( subscription, SIGNAL(subscriptionTick()), this, SLOT(handleSubscriptionTick()) );
		connect( subscription, SIGNAL(destroyMe()), this, SLOT(destroySubscription()) );
		subscription->start();
		return true;
	}
	else
	{
		subscription->deleteLater();
		return false;
	}
}

void ClientSubscriptionManager::destroySubscription( ClientSubscription *subscription )
{
	if( subscription == 0 )
	{
		if( sender() )
			{ subscription = (ClientSubscription*)sender(); }
		else
		{
			error( QtWarningMsg, "Can't identify subscription", "destroySubscription()" );
			return;
		}
	}

	for( int i=0; i<mSubscriptions.size(); ++i )
	{
		if( mSubscriptions.at(i) == subscription )
		{
			subscription->stop();
			subscription->deleteLater();
			mSubscriptions.removeAt(i);
		}
	}
}

void ClientSubscriptionManager::unSubscribe( ClientConnectionManagerBase *client, const QString &hwInterface, const QString &variable )
{
	for( int i=0; i<mSubscriptions.size(); ++i )
	{
		if( mSubscriptions.at(i)->getClient() != client )
			{ continue; }
		if(
		   ( variable == mSubscriptions.at(i)->getVariable() && hwInterface == mSubscriptions.at(i)->getHwInterface() ) ||
		   ( variable == "*" && hwInterface == "*" ) ||
		   ( variable == "*" && hwInterface == mSubscriptions.at(i)->getHwInterface() )
		){
			destroySubscription( mSubscriptions.at(i) );
		}
	}
}

void ClientSubscriptionManager::handleSubscriptionTick()
{
	ClientSubscription *subscription = (ClientSubscription*)sender();
	emit subscriptionFeedRequest( subscription );
}

bool ClientSubscriptionManager::moreSpecificSubscriptionExists( const DeviceStateVariable *variable, const ClientSubscription *subscription ) const
{
	for( int i=0; i<mSubscriptions.size(); ++i )
	{
		if( mSubscriptions.at(i) != subscription && subscription->includes(*(mSubscriptions.at(i))) && mSubscriptions.at(i)->includes(variable) )
			{ return true; }
	}
	return false;
}
