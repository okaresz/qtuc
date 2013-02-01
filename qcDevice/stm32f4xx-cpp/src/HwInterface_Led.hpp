#ifndef HWINTERFACE_LED_H
#define HWINTERFACE_LED_H

#include "QtuC_Tools.hpp"
#include "HAL_QIO.hpp"

namespace HwInterface
{

namespace Conf
{
	/// This is a full configuration set, you can safely change it without modifying the implementation code.
	namespace Led
	{
		namespace Pin
		{
			namespace stmGreen {
				uint8_t const bit = GPIO_Pin_12_Bit;
				GPIO_TypeDef *const port = GPIOD;
			}
			namespace stmOrange {
				uint8_t const bit = GPIO_Pin_13_Bit;
				GPIO_TypeDef *const port = GPIOD;
			}
			namespace stmRed {
				uint8_t const bit = GPIO_Pin_14_Bit;
				GPIO_TypeDef *const port = GPIOD;
			}
			namespace stmBlue {
				uint8_t const bit = GPIO_Pin_15_Bit;
				GPIO_TypeDef *const port = GPIOD;
			}
			// If you add a new LED, don't forget to update Led::MLedCount, Led::ledId_t, and Led::mLedList!
		}
	}
}


/** Hardware interface to manage some leds.
 *	Mainly for debug purposes...<br>
 *	To add a led, increase HwI__Led_LEDCOUNT, define the new macros for the pin and port,
 *	add the new pin name to ledPins, and set its port and pin in the constructor.*/
class Led
{
private:
	Led(){}
public:
	/// Name of the interface
	static const char InterfaceName[];

	// Common hwInterface functions ----------------

	static bool init();
	static bool start();
	static inline bool initialized()
		{ return mInitialized; }
	static inline bool started()
		{ return mStarted; }
	static bool reinit() { return init(); };
	static bool execCmd( QtuC::cmdType_t type, char *var, char *arg );

	/** List of leds.
	 *	This enum is used for indexing the array of available leds,
	 *	so this must start with 0 and increase one-by-one.*/
	enum ledId_t {
		stmGreen = 0,
		stmOrange,
		stmRed,
		stmBlue,
	};

	/** Type for representing a led.
	 *	name: Name of the led (use this in the device commands).<br>
	 *	pin: Pin of the led.*/
	struct led_t {
		const char *name;
		HAL::QIO pin;
	};

	/** Set a led.
	 *	@param led The led pin to set.
	 *	@param on If true, the led will light up, if false, it will be turned off.*/
	static void set( ledId_t led, bool on );

	/** Turn a led on.
	 *	@param led The led to turn on.*/
	static inline void on( ledId_t led ) { HwInterface::Led::set(led, true); }

	/** Turn a led off.
	*	@param led The led to turn off.*/
	static inline void off( ledId_t led ) { HwInterface::Led::set(led, false); }

private:
	static const uint8_t MLedCount = 4;
	static led_t mLedList[MLedCount];

	static bool mInitialized;
	static bool mStarted;
};

}	//HwInterface::
#endif // HWINTERFACE_LED_H
