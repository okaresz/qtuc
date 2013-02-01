#ifndef SYSTIME_H
#define SYSTIME_H

#include "stm32f4xx_conf.h"

// These must be in global scope
#define Conf_SysTime_IRQHandler TIM5_IRQHandler(void)
extern "C" void Conf_SysTime_IRQHandler;

namespace Conf
{
	/// This is a full configuration set, you can safely change it without modifying the implementation code.
	namespace SysTime
	{
		TIM_TypeDef* const timer = TIM5;
		uint32_t const timerAPBClock = 84000000L;
		uint32_t const timerRCC = RCC_APB1Periph_TIM5;
		void (* const timerRCCClockCmd)(uint32_t, FunctionalState) = RCC_APB1PeriphClockCmd;
		IRQn const timerIRQ = TIM5_IRQn;
		uint16_t const prescaler = 84;	//1us / tick, thats 1MHz
		uint32_t const period = 4294000000UL;
		uint32_t const clockFreq = timerAPBClock / prescaler;
	}
}

/** Class to measure time.
 *	Initializes a timer to measure the system uptime (time since system startup).
 *	Resolution can be configured.
 *
 *	By default, the timer clock is 1MHz, so a counter increment is 1us. This means an overflow every 4295s (71.58min).
 *	With the 16bit overflow counter, approximately 9years of continuous timekeepeing is possible.*/
class SysTime
{
private:
	SysTime(){}
public:
	friend void Conf_SysTime_IRQHandler;

	/// Initialize SysTime.
	static void init(void);

	/// Start SysTime timer.
	static void start(void);

	/// Type to represent a system timestamp (a given point in time).
	struct sysTimeStamp_t
	{
		uint32_t cnt;
		uint16_t ovf;
	};

	static sysTimeStamp_t const NullTime;

	/// Type to represent passed time since a timestamp.
	struct sysTime_t
	{
		uint32_t sec;		///< Number of seconds since the timestamp.
		uint32_t usec;	///< Number of microseconds passed in the last sec (max 999999).
	};

	/** Get a system timestamp.
	 *	@return A system timestamp with the current time.*/
	static sysTimeStamp_t const getTimeStamp(void);

	/** Get ticks since the passed timestamp.
	 *	@param timeStamp The start timestamp.
	 *	@param usSinceTimeStamp This param will be filled with the number of ticks since the timestamp, as long as it can be represented with a 32bit integer.
	 *	If the tick count since the timestamp is more than 2^32, then then this parameter will be 0. See also the return value.
	 *	@return True if the ticks since the passed timestamp is less than 2^32, false if it's more than that, and this function cannot return the passed time correctly. In this case, see getTimeSince().*/
	static bool getUsSince( sysTimeStamp_t const &timeStamp, uint32_t &usSinceTimeStamp );

	/** Get time since a timestamp.
	*	@param timeStamp The start timestamp.
	*	@return System time passed since the given timestamp.*/
	static sysTime_t getTimeSince( sysTimeStamp_t const &timeStamp );

private:
	static uint16_t mOvfCnt;
};

#endif /* SYSTIME_H */
