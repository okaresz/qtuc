#include "ClientSubscription.h"
#include <QTimerEvent>

using namespace QtuC;

unsigned int ClientSubscription::maxSubscriptionFrequency = 1000;

ClientSubscription::ClientSubscription(ClientConnectionManagerBase *client, unsigned int freq, const QString &hwInterface, const QString &variable, QObject *parent) :
	ErrorHandlerBase(parent),
	mClient(client),
	mVariable(variable),
	mHwInterface(hwInterface),
	mFrequency(freq),
	mTimerId(0)
{

	errorDetails_t errDet;
	errDet.insert( "variable", mVariable );
	errDet.insert( "hwInterface", mHwInterface );
	errDet.insert( "freq", QString::number(mFrequency) );

	if( mFrequency == 0 )
		{ error( QtWarningMsg, "Requested subscription frequency zero, doing nothing", "ClientSubscription()", errDet ); }

	if( mFrequency > maxSubscriptionFrequency )
		{ error( QtWarningMsg, QString("Requested subscription frequency is more than the maximum of %1 Hz").arg(QString::number(maxSubscriptionFrequency)), "ClientSubscription()", errDet ); }

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

	mTimerId = startTimer( (int)(1000.0/mFrequency+0.5) );
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
	valid = valid && mFrequency > 0;
	valid = valid && mFrequency < maxSubscriptionFrequency;
	valid = valid && !( mHwInterface.isEmpty() && !mVariable.isEmpty() );
	return valid;
}

void ClientSubscription::timerEvent(QTimerEvent *timerEvent)
{
	timerEvent->accept();
	emit subscriptionTick();
}
