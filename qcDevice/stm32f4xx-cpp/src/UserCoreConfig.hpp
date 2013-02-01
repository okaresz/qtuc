/** @file
 *	Core configurations made by the user.*/

#ifndef USERCORECONFIG_H
#define USERCORECONFIG_H

#include <misc.h>
#include "stm32f4xx_gpio.h"

//### Platform-specific ########################################################
/** @name IO Config.
 *	Define platform-specific IO pins.
 *	These pins are initialized in UserCoreConfig().
 *	@{*/

#define CoreCfg_Led_Fault_Port	GPIOD
#define CoreCfg_Led_Fault_Pin	GPIO_Pin_14			///< Red LED on STM32F4DISCOVERY board
#define CoreCfg_Led_Fault_PortRCC RCC_AHB1Periph_GPIOD

#define CoreCfg_Led_DefaultISR_Port	GPIOD
#define CoreCfg_Led_DefaultISR_Pin	GPIO_Pin_13		///< Orange LED on STM32F4DISCOVERY board
#define CoreCfg_Led_DefaultISR_PortRCC RCC_AHB1Periph_GPIOD

#define CoreCfg_Led_Resource_Port	GPIOD
#define CoreCfg_Led_Resource_Pin	GPIO_Pin_12		///< Green LED on STM32F4DISCOVERY board
#define CoreCfg_Led_Resource_PortRCC RCC_AHB1Periph_GPIOD

#define CoreCfg_Led_Debug_Port	GPIOD
#define CoreCfg_Led_Debug_Pin	GPIO_Pin_15			///< Blue LED on STM32F4DISCOVERY board
#define CoreCfg_Led_Debug_PortRCC RCC_AHB1Periph_GPIOD

/// @}
//###########################################################

/** @name Convenience macros for switching the debug led on/off.
*	@{*/
#define DebugLed_ON() (CoreCfg_Led_Debug_Port->BSRRL = CoreCfg_Led_Debug_Pin)
#define DebugLed_OFF() (CoreCfg_Led_Debug_Port->BSRRH = CoreCfg_Led_Debug_Pin)
/// @}

/** @name Resource indicator helpers.
 *	To monitor interrupt CPU time, use these helper macros in an interrupt.
 *	The resource LED will be switched on while the interrupt is being served.
 *	@{*/
#define ISR_Start() (CoreCfg_Led_Resource_Port->BSRRL=CoreCfg_Led_Resource_Pin)	///< Indicate the start of an interrupt.
#define ISR_End() (CoreCfg_Led_Resource_Port->BSRRH=CoreCfg_Led_Resource_Pin)		///< Indicate the end of an interrupt.
/// @}

/** Execute initial configurations.
 *	You should execute this before branching to main() (usually you have to modify the startup assmebly for this),
 *	or call this in main before any other instruction.*/
void UserCoreConfig();

/** Default System Fault handler.*/
extern "C" void defaultFaultHandler(void);

/** Default interrupt handler.
 *	Default handler jumps to an infinite loop... This handler however lights up a led, which is a much more debug-friendly solution.
 *	Feel free to customize.*/
extern "C" void defaultInterruptHandler(void);

#endif /* USERCORECONFIG_H */
