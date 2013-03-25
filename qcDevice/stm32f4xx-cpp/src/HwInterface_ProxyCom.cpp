#include "HwInterface_ProxyCom.hpp"
#include <errno.h>
#include <sys/unistd.h>
#include <string.h>
#include <stdio.h>

/// The interrupt handler routine of USART.
extern "C" void Conf_ProxyCom_USART_IRQHandler
{
	if( USART_GetITStatus(HwInterface::Conf::ProxyCom::USART::module, USART_IT_RXNE) != RESET )
	{
		/* Read one byte from the receive data register, and give it to ProxyCom */
		HwInterface::ProxyCom::handleNewData( USART_ReceiveData(HwInterface::Conf::ProxyCom::USART::module) );
	}
}

using namespace HwInterface;
using namespace HwInterface::Conf::ProxyCom;

const char *ProxyCom::InterfaceName = QtuC::Conf::proxyInterfaceName;
bool ProxyCom::mInitialized = false;
bool ProxyCom::mStarted = false;

char ProxyCom::mReceiveBuffer[][ProxyCom::MReceiveBufferCmdSize] = {};

uint8_t ProxyCom::mReceiveBufferPtr = 0;
uint8_t ProxyCom::mReceiveBufferCmdWPtr = 0;
uint8_t ProxyCom::mReceiveBufferCmdRPtr = 0;

bool ProxyCom::init()
{
	if( mInitialized ) { return true; }

	RCC_AHB1PeriphClockCmd( Pin::TX::portRCC | Pin::RX::portRCC, ENABLE);
	/// @todo func ptr to RCC init funct!
	RCC_APB2PeriphClockCmd( USART::moduleRCC, ENABLE );

	GPIOB->BSRRH = Pin::TX::pin | Pin::RX::pin;

	GPIO_InitTypeDef ioInit;
	ioInit.GPIO_Speed = GPIO_Speed_100MHz;
	ioInit.GPIO_Mode = GPIO_Mode_AF;
	ioInit.GPIO_OType = GPIO_OType_PP;
	ioInit.GPIO_PuPd = GPIO_PuPd_UP;

	ioInit.GPIO_Pin = Pin::TX::pin;
	GPIO_Init( Pin::TX::port, &ioInit );
	ioInit.GPIO_Pin = Pin::RX::pin;
	GPIO_Init( Pin::RX::port, &ioInit );

	GPIO_PinAFConfig( Pin::TX::port, Pin::TX::AFPinSource, USART::AFMap );
	GPIO_PinAFConfig( Pin::RX::port, Pin::RX::AFPinSource, USART::AFMap );

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = USART::baudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init( USART::module, &USART_InitStructure );

	/* Enable the USART1 Interrupt */
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART::IRQCh;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	mInitialized = true;
	return true;
}

bool ProxyCom::start()
{
	if( mStarted ) { return true; }

	USART_ITConfig( USART::module, USART_IT_RXNE, ENABLE );
	USART_Cmd( USART::module, ENABLE );

	mStarted = true;

	// Send a newline to flush all weird characters that may be present after startup
	putEndl();

	return true;
}

void ProxyCom::stop()
{
	USART_ITConfig( USART::module, USART_IT_RXNE, DISABLE );
	USART_Cmd( USART::module, DISABLE );
	mStarted = false;
}

bool ProxyCom::execCmd( QtuC::cmdType_t type, char *var, char *arg )
{
	//Nothing here.
	return true;
}

// For this to work, the RX interrupt should be disabled
//unsigned char HwiProxyCom::getChar()
//{
//	while( USART_GetFlagStatus(USART::module, USART_FLAG_RXNE) == RESET );
//	return (unsigned char)USART_ReceiveData(USART::module);
//}

void ProxyCom::putChar( char const &ch )
{
	if( !mStarted ) { return; }
	USART_SendData( USART::module, (u8)ch );
	// Wait for the data register to be transferred into the USART shift register
	for(; USART_GetFlagStatus(USART::module, USART_FLAG_TXE) == RESET ;);
}

uint16_t ProxyCom::print( const char* str )
{
	if( !mStarted ) { return 0; }

	uint16_t chcnt = 0;
	IRQDIS();
	while( *(str+chcnt) )
	{
		ProxyCom::putChar( *(str+chcnt) );
		chcnt++;
	}
	IRQEN();

	return chcnt;
}

void ProxyCom::handleNewData( uint16_t const &newData )
{
	// skip carriage return
	if( newData == 0xd ) return;

	// skip other meaningless bytes
	if( newData == 0xff ) return;

	if( newData == 0xa )
	{
		if( getIncReceiveBufferCmdPtr(mReceiveBufferCmdWPtr) == mReceiveBufferCmdRPtr )
			{ QtuC::Tools::sendMessage( QtuC::msgError, "Cmd buf ovf, slow down!" ); }
		else
		{
			mReceiveBuffer[mReceiveBufferCmdWPtr][mReceiveBufferPtr] = '\0';
			mReceiveBufferPtr = 0;
			incReceiveBufferCmdPtr(mReceiveBufferCmdWPtr);
		}
	}
	else
	{
		if( mReceiveBufferPtr < MReceiveBufferCmdSize-1 )
		{
			mReceiveBuffer[mReceiveBufferCmdWPtr][mReceiveBufferPtr++] = (char)newData;
		}
		else
			{ QtuC::Tools::sendMessage( QtuC::msgError, "Too long cmd, buf ovf" ); }
	}
}

uint8_t ProxyCom::getIncReceiveBufferCmdPtr( const uint8_t &ptr )
{
	if( ptr < MReceiveBufferCmdCount-1 )
		{ return ptr+1; }
	else
		{ return 0; }
}

uint8_t ProxyCom::getNextCommand(char* cmd)
{
	if( mReceiveBufferCmdRPtr != mReceiveBufferCmdWPtr )
	{
		strcpy( cmd, mReceiveBuffer[mReceiveBufferCmdRPtr] );
		incReceiveBufferCmdPtr(mReceiveBufferCmdRPtr);
		return strlen(cmd);
	}
	else
		{ return 0; }
}


extern "C"
{

/// Read implementation of the newlib stubs
int _read(int file, char *ptr, int len)
	{ return 0; }
/// Write implementation of the newlib stubs
int _write(int file, char *ptr, int len)
	{ return 0; }

// If you'd like to implement it...
/*
/// Read implementation of the newlib stubs
int _read(int file, char *ptr, int len) {
    int n;
    int num = 0;
    switch (file)
    {
		case STDIN_FILENO:
			for( n = 0; n < len; n++ )
			{
				if( !HwiProxyCom::instance() ) break;

				// ProxyCom doesn't have a getChar method, as it is fully RX interrupt driven.
				char c = HwInterface::ProxyCom::

				*ptr++ = c;
				num++;
			} break;
		default:
			errno = EBADF;
			return -1;
    }
    return num;
}

/// Write implementation of the newlib stubs
int _write(int file, char *ptr, int len)
{
    int n;
    switch (file)
    {
		case STDOUT_FILENO:
			for( n = 0; n < len; n++ )
			{
				if( !HwInterface::ProxyCom::started() ) break;
				HwInterface::ProxyCom::putChar(*ptr++ & (uint16_t)0x01FF);
			}
			break;
		case STDERR_FILENO:
			for (n = 0; n < len; n++)
			{
				if( !HwInterface::ProxyCom::started() ) break;
				HwInterface::ProxyCom::putChar(*ptr++ & (uint16_t)0x01FF);
			}
			break;
		default:
			errno = EBADF;
			return -1;
    }
    return len;
}
*/
}
