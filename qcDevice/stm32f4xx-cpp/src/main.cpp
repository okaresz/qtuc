#include <string.h>
#include "UserCoreConfig.hpp"
#include "SysTime.hpp"
#include "QtuC_Tools.hpp"
#include "QtuC_Interfaces.hpp"
#include "HwInterface_ProxyCom.hpp"
#include "HwInterface_Led.hpp"
#include "delay.hpp"

/** Device parameters.
 *	Can be sent to QtuC with QtuC::Tools::sendGreeting().
 *	Can be extended with more parameters freely.*/
char const *deviceParams[] = {
	"name", "qcDevice",
	"desc", "Example code for a qcDevice implementation",
	"platform", "STM32F4xx",
	"project", "QtuC",
	"timeTicksPerMs", "1000",
	"positiveAck", "false",
	0	// this terminating NULL pointer is necessary
};

int main(void)
{
	// Core configurations
	UserCoreConfig();

	// start system timer
	SysTime::init();
	SysTime::start();

	// Register the device interfaces
	QtuC::Interfaces::regInterface( HwInterface::ProxyCom::InterfaceName, HwInterface::ProxyCom::execCmd );
	QtuC::Interfaces::regInterface( HwInterface::Led::InterfaceName, HwInterface::Led::execCmd );
	//Don't forget to increase QtuC::Interfaces::MMaxInterfaceCount if necessary!

	// Initialize and start interfaces
	HwInterface::ProxyCom::init();
	HwInterface::ProxyCom::start();

	HwInterface::Led::init();
	HwInterface::Led::start();

	// We are ready, send device greeting message
	QtuC::Tools::sendGreeting( deviceParams, "Hi proxy!" );

	// The main loop
	char cmd[QtuC::Conf::maxCommandLength];
	while(1)
	{
		// Receive next pending command and route it to the proper interface
		if( HwInterface::ProxyCom::isCommandReady() )
		{
			HwInterface::ProxyCom::getNextCommand(cmd);
			QtuC::Interfaces::routeCmd(cmd);
		}

		// Some more stuff in the main loop...

	}

	return 0;
}


// Print implementation for QtuC::Tools.
void QtuC::print( const char *str )
	{ HwInterface::ProxyCom::print( str ); }

// PutChar implementation for QtuC::Tools.
void QtuC::putChar( const char c )
	{ HwInterface::ProxyCom::putChar(c); }
