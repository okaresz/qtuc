#include "HwInterface_Led.hpp"

using namespace HwInterface;
using namespace HwInterface::Conf::Led;

const char Led::InterfaceName[] = {"led"};
bool Led::mInitialized = false;
bool Led::mStarted = false;

Led::led_t Led::mLedList[] =
{
	{ "stmGreen", HAL::QIO( Pin::stmGreen::port, Pin::stmGreen::bit ) },
	{ "stmOrange", HAL::QIO( Pin::stmOrange::port, Pin::stmOrange::bit ) },
	{ "stmRed", HAL::QIO( Pin::stmRed::port, Pin::stmRed::bit ) },
	{ "stmBlue", HAL::QIO( Pin::stmBlue::port, Pin::stmBlue::bit ) }
};

bool Led::init()
{
	if( mInitialized ) { return true; }

	for( uint8_t i=0; i<MLedCount; ++i )
		{ mLedList[i].pin.init( GPIO_Mode_OUT ); }
	mInitialized = true;
	return true;
}

bool Led::start()
{
	if( mStarted ) { return true; }

	mStarted = true;
	return true;
}

void Led::set( ledId_t led, bool on )
{
	if( on )
		{ mLedList[led].pin.set(); }
	else
		{ mLedList[led].pin.reset(); }

}

bool Led::execCmd( QtuC::cmdType_t type, char *var, char *arg )
{
	if( type == QtuC::cmdSet )
	{
		bool state = QtuC::isTrue( arg );

		for( uint8_t i=0; i<MLedCount; ++i )
		{
			if( QtuC::Tools::isArg( var, mLedList[i].name ) )
			{
				set( (ledId_t)i, state );
				return true;
			}
		}
	}

	return false;
}
