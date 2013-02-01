#include "SysTime.hpp"
#include "UserCoreConfig.hpp"

/** Interrupt handler of the SysTime timer module.
 *	This interrupt has to be as lean as possible, so instead checking if this is really an update  interrupt caused by an overflow,
 *	ensure that only this type of interrupt has been enabled for this timer,
 *	This handler increases SysTime::mOvfCount overflow counter by one.*/
extern "C" void Conf_SysTime_IRQHandler
{
	if( TIM_GetITStatus( Conf::SysTime::timer, TIM_IT_Update ) )
	{
		++SysTime::mOvfCnt;
		TIM_ClearITPendingBit( Conf::SysTime::timer, TIM_IT_Update );
	}
}

using namespace Conf::SysTime;

uint16_t SysTime::mOvfCnt = 0;
SysTime::sysTimeStamp_t const SysTime::NullTime = {0,0};

void SysTime::init(void)
{
	timerRCCClockCmd( timerRCC, ENABLE );
	TIM_DeInit(timer);
	TIM_InternalClockConfig(timer);

	// ..and the auto-reload preload.
	TIM_ARRPreloadConfig( timer, ENABLE );

	// TIM5 on APB1, clock is 84MHz
	TIM_TimeBaseInitTypeDef timtb;
	timtb.TIM_CounterMode = TIM_CounterMode_Up;
	timtb.TIM_Period = period;
	timtb.TIM_Prescaler = prescaler;
	TIM_TimeBaseInit( timer, &timtb );

	TIM_UpdateRequestConfig( timer, TIM_UpdateSource_Regular );
	TIM_ITConfig( timer, TIM_IT_Update, ENABLE );
	TIM_ClearITPendingBit( timer, TIM_IT_Update );	//WTF?? why is this necessary?

	NVIC_InitTypeDef nvicConf;
	nvicConf.NVIC_IRQChannel = timerIRQ;
	nvicConf.NVIC_IRQChannelCmd = ENABLE;
	nvicConf.NVIC_IRQChannelPreemptionPriority = 0;
	nvicConf.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init( &nvicConf );
}

void SysTime::start(void)
{
	TIM_GenerateEvent( timer, TIM_EventSource_Update );
	TIM_ClearITPendingBit( timer, TIM_IT_Update );
	TIM_Cmd( timer, ENABLE );
}

SysTime::sysTimeStamp_t const SysTime::getTimeStamp(void)
{
	// get tick count as soon as possible
	uint32_t ticks = timer->CNT;
	sysTimeStamp_t stamp = { ticks, mOvfCnt };
	return stamp;
}

bool SysTime::getUsSince( sysTimeStamp_t const &timeStamp, uint32_t &usSinceTimeStamp )
{
	// get tick and ovf count as soon as possible
	uint32_t cnt = timer->CNT;
	uint16_t ovf = mOvfCnt;

	if( timeStamp.ovf > ovf )
	{
		usSinceTimeStamp = 0;
		return false;
	}

	if( ovf == timeStamp.ovf )
	{
		// future stamp?
		if( !(cnt < timeStamp.cnt) )
		{
			usSinceTimeStamp = cnt - timeStamp.cnt;
			return true;
		}
	}
	else
	{
		if( ovf-1 == timeStamp.ovf && cnt < timeStamp.cnt )
		{
			usSinceTimeStamp = timer->ARR - timeStamp.cnt + cnt + 1;
			return true;
		}
	}

	usSinceTimeStamp = 0;
	return false;
}

SysTime::sysTime_t SysTime::getTimeSince( sysTimeStamp_t const &timeStamp )
{
	// get tick and ovf count as soon as possible
	uint32_t ticks = timer->CNT;
	uint16_t ovf = mOvfCnt;
	sysTime_t timeSince;

	uint32_t tickDiff, ovfDiff;

	// this calculates the passed time in encoder ticks / ovf-s
	/*if( ticks >= timeStamp.ticks )
	{
		timeSince.ticks = ticks-timeStamp.ticks;
		timeSince.tickOvf = ovf-timeStamp.ovf;
	}
	else
	{
		timeSince.ticks = timer->ARR - timeStamp.cnt + cnt + 1;
		/// @todo if this is <0, chances are an tickOvf overflow happened,
		timeSince.tickOvf = ovf-timeStamp.ovf-1;
	}*/

	// calc sec, usec
	if( ticks >= timeStamp.cnt )
	{
		tickDiff = ticks-timeStamp.cnt;
		ovfDiff = ovf-timeStamp.ovf;
	}
	else
	{
		tickDiff = timer->ARR - timeStamp.cnt + 1 + ticks;
		/// @todo if this is <0, chances are an tickOvf overflow happened,
		ovfDiff = ovf-timeStamp.ovf-1;
	}

	timeSince.sec = ovfDiff * (period / clockFreq) + tickDiff / clockFreq;
	timeSince.usec = tickDiff % clockFreq;

	return timeSince;
}
