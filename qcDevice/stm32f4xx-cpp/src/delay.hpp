/*	@file
 *	Busy delay functions with SysTick.*/

#ifndef DELAY_HPP
#define DELAY_HPP

#include <stm32f4xx.h>

/** Busy delay x microseconds.
 *	This function uses SysTick to count one us.
 *	@param Microseconds to wait before return.*/
void delay_us( volatile uint32_t usCount )
{
       SysTick->LOAD = SysTick->VAL = ((SystemCoreClock/1000000) & SysTick_LOAD_RELOAD_Msk) - 1;
       SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

       while(usCount--)
               while(!(SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk));

       SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/** Busy delay x milliseconds.
 *	This function uses SysTick to count one ms.
 *	@param Milliseconds to wait before return.*/
void delay_ms( volatile uint32_t msCount )
{
       SysTick->LOAD = SysTick->VAL = ((SystemCoreClock/1000) & SysTick_LOAD_RELOAD_Msk) - 1;
       SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;

       while(msCount--)
               while(!(SysTick->CTRL&SysTick_CTRL_COUNTFLAG_Msk));

       SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

#endif // DELAY_HPP
