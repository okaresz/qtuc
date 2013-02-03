#ifndef HWINTERFACE_PROXYCOM_H
#define HWINTERFACE_PROXYCOM_H

#include "QtuC_Tools.hpp"

// These must be in global scope
#define Conf_ProxyCom_USART_IRQHandler USART1_IRQHandler(void)
extern "C" void Conf_ProxyCom_USART_IRQHandler;

namespace HwInterface
{

namespace Conf
{
	/// This is a full configuration set, you can safely change it without modifying the implementation code.
	namespace ProxyCom
	{
		namespace Pin
		{
			namespace TX {
				uint32_t const pin = GPIO_Pin_6;
				GPIO_TypeDef *const port = GPIOB;
				uint32_t const portRCC = RCC_AHB1Periph_GPIOB;
				uint8_t const AFPinSource = GPIO_PinSource6;
			}
			namespace RX {
				uint32_t const pin = GPIO_Pin_7;
				GPIO_TypeDef *const port = GPIOB;
				uint32_t const portRCC = RCC_AHB1Periph_GPIOB;
				uint8_t const AFPinSource = GPIO_PinSource7;
			}
		}
		namespace USART
		{
			USART_TypeDef* volatile const module = USART1;
			uint32_t const moduleRCC = RCC_APB2Periph_USART1;
			uint8_t const AFMap = GPIO_AF_USART1;
			IRQn_Type const IRQCh = USART1_IRQn;
			uint32_t const baudRate = 115200;
		}
	}
}

/** Hardware interface for communicating with qcProxy.
 *	Use ProxyCom::isCommandReady() to check for new incoming command.
 *	Retrieve the received command with ProxyCom::getCommand().<br>
 *	The commands are received to a circular buffer with a size of HwI__ProxyCom_ReceiveBuffer_CmdCount.
 *	Each command buffer can store a command up to HwI__ProxyCom_ReceiveBuffer_Size.
 *	If the circular buffer is full, an error message is sent with QtuC::Tools::sendMessage().*/
class ProxyCom
{
private:
	/// Privet c'tor. This is  a static class, don't instantiate.
	ProxyCom(){}

public:
	/// Let's be friends with the USART ISR
	friend void ::Conf_ProxyCom_USART_IRQHandler;

	/// Name of this hardware interface.
	static const char *InterfaceName;

	/** Initialize the hardware interface.
	 *	Place one-time initialization code here.*/
	static bool init();

	/** Start interface.
	 *	If the interface can be used start/stop fashion, the start part is here.*/
	static bool start();

	/** Stop the interface.
	 *	If the interface can be used start/stop fashion, the start part is here.*/
	static void stop();

	static inline bool initialized()
		{ return mInitialized; }
	static inline bool started()
		{ return mStarted; }

	/** Re-initialize hardware interface.
	 *	Don't forget to call start() after this, if necessary.*/
	static bool reinit() { return init(); };

	/** Execute a device command.
	 *	This is the entry point to the interface. This method gets called if a device command is received for this interface.
	 *	This command must be static (to be callable without an object scope). Inside the method, use the instance() method of the interface.
	 *	@param type Type of the command.
	 *	@param var Command variable.
	 *	@param arg Command argument.
	 *	@return True if command has executed successfully, false otherwise.*/
	static bool execCmd( QtuC::cmdType_t type, char *var, char *arg );

	/** Get the next pending command from the receive buffer.
	 *	@param cmd A buffer to copy the command to.
	 *	@return The length of the command.*/
	static uint8_t getNextCommand(char* cmd);

	/** Check if there is a new received command.
	 *	@return True if there is a pending command in the receive buffer, false otherwise.*/
	static inline bool isCommandReady()
		{ return mReceiveBufferCmdRPtr != mReceiveBufferCmdWPtr; }

	/** Print a character.
	 *	Only sending id interface was started...........
	 *	@param ch Character to print.*/
	static void putChar(char const &ch);

	/** Print a string.
	 *	Send string to qcProxy.
	 *	Only sending id interface was started...........
	 *	@param str The string to send.
	 *	@return Number of characters sent.*/
	static uint16_t print( const char* str );

	/// Print a newline.
	static inline void putEndl()
		{ ProxyCom::putChar('\n'); }

private:

	/** Get a character from the USART1 buffer.
	 *	@return The character read.*/
	//unsigned char getChar();

	/** Handle received data from QtuC Proxy.
	 *	Called from the USART IRQ handler.
	 *	@param newData The data to process.*/
	static void handleNewData( uint16_t const &newData );

	/** Get the increased value of an input buffer index.
	 *	The index itself is left unchanged.
	 *	@param ptr The index to get the increased value for.
	 *	@return The increased value.*/
	static uint8_t getIncReceiveBufferCmdPtr( const uint8_t &ptr );

	/** Increase an input buffer index.
	 *	@param ptr The index to increase.*/
	static inline void incReceiveBufferCmdPtr( uint8_t &ptr )
		{ ptr = getIncReceiveBufferCmdPtr(ptr); }


	static bool mInitialized;
	static bool mStarted;

	static uint8_t const MReceiveBufferCmdCount = 4;	///< Number of commands that can be stored in the receive buffer.
	static uint8_t const MReceiveBufferCmdSize = QtuC::Conf::maxCommandLength;	///< Size of the command receive buffer.

	static char mReceiveBuffer[MReceiveBufferCmdCount][MReceiveBufferCmdSize];	///< The circular input command buffer.
	static uint8_t mReceiveBufferCmdWPtr, mReceiveBufferCmdRPtr, mReceiveBufferPtr;	///< Pointer indexes for the input buffer.
};

}	//HwInterface::
#endif // HWINTERFACE_PROXYCOM_H
