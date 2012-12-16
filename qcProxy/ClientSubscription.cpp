#include "ClientSubscription.h"
#include <QTimerEvent>

using namespace QtuC;

quint32 ClientSubscription::minSubscriptionInterval = 20;

ClientSubscription::ClientSubscription(ClientConnectionManagerBase *client, quint32 interval, const QString &hwInterface, const QString &variable, QObject *parent) :
	ErrorHandlerBase(parent),
	mClient(client),
	mVariable(variable),
	mHwInterface(hwInterface),
	mInterval(interval),
	mTimerId(0)
{

	errorDetails_t errDet;
	errDet.insert( "variable", mVariable );
	errDet.insert( "hwInterface", mHwInterface );
	errDet.insert( "interval", QString::number(mInterval) );

	if( mInterval == 0 )
		{ error( QtWarningMsg, "Requested subscription interval is zero, doing nothing", "ClientSubscription()", errDet ); }

	if( mInterval < minSubscriptionInterval )
		{ error( QtWarningMsg, QString("Requested subscription interval is less than the minimum of %1ms").arg(QString::number(minSubscriptionInterval)), "ClientSubscription()", errDet ); }

	if( mHwInterface.isEmpty() && !mVariable.isEmpty() )
		{ error( QtWarningMsg, "Cannot specify subscription variable when hardware interface is empty", "ClientSubscription()", errDet ); }

	connect( mClient, SIGNAL(clientDisconnected()), this, SIGNAL(destroyMe()) );
}

ClientSubscription::~ClientSubscription()
{
	debug( debugLevelVeryVerbose, "Destroyed", "~ClientSubscription()" );
}

bool ClientSubscription::start()
{
	if( mTimerId != 0 )
	{
		error( QtWarningMsg, "Timer is already running, stop it first", "start()" );
		return false;
	}

	if( !isValid() )
	{
		error( QtWarningMsg, "Subscription is invalid, not started", "start()" );
		return false;
	}

	mTimerId = startTimer( mInterval );
	return ( mTimerId != 0 );
}

void ClientSubscription::stop()
{
	if( mTimerId != 0 )
	{
		killTimer( mTimerId );
		mTimerId = 0;
	}
}

bool ClientSubscription::operator ==( const ClientSubscription &otherSubscription) const
{
	return ( mHwInterface == otherSubscription.getHwInterface() && mVariable == otherSubscription.getVariable() );
}

bool ClientSubscription::includes(const ClientSubscription &other) const
{
	if( *this == other )
		{ return true; }
	if( mHwInterface.isEmpty() && mVariable.isEmpty() )
		{ return true; }
	if( mHwInterface == other.mHwInterface && mVariable.isEmpty() )
		{ return true; }
	return false;
}

bool ClientSubscription::includes(const DeviceStateVariable *variable) const
{
	if( mHwInterface.isEmpty() && mVariable.isEmpty() )
		{ return true; }
	if( mHwInterface == variable->getHwInterface() && mVariable.isEmpty() )
		{ return true; }
	if( mHwInterface == variable->getHwInterface() && mVariable == variable->getName() )
		{ return true; }
	return false;
}

bool ClientSubscription::isValid()
{
	bool valid = true;
	valid = valid && mInterval > minSubscriptionInterval;
	valid = valid && !( mHwInterface.isEmpty() && !mVariable.isEmpty() );
	return valid;
}

void ClientSubscription::timerEvent(QTimerEvent *timerEvent)
{
	timerEvent->accept();
	emit subscriptionTick();
}
