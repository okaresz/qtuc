#include "HAL_QIO.hpp"

using namespace HAL;

GPIO_InitTypeDef QIO::InitStruct =
{
	0,
	GPIO_Mode_OUT,
	GPIO_Speed_100MHz,
	GPIO_OType_PP,
	GPIO_PuPd_NOPULL
};

QIO::QIO( GPIO_TypeDef* const &ioPort, uint8_t const &ioPinBit ) :
	mPort(ioPort),
	mPinBit(ioPinBit),
	mPin(1<<ioPinBit)
{}

void QIO::init( GPIO_TypeDef* const &GPIOx, uint32_t const &pin, const GPIOMode_TypeDef &ioMode, const GPIOOType_TypeDef &oType, const GPIOPuPd_TypeDef &ppType, const GPIOSpeed_TypeDef &ioSpeed, bool initClock )
{
	if( initClock )
		{ enablePortRCC(GPIOx); }
	InitStruct.GPIO_Pin = pin;
	InitStruct.GPIO_Mode = ioMode;
	InitStruct.GPIO_Speed = ioSpeed;
	InitStruct.GPIO_OType = oType;
	InitStruct.GPIO_PuPd = ppType;
	GPIO_Init( GPIOx, &InitStruct );
}

void QIO::init( const GPIOMode_TypeDef &ioMode, const GPIOOType_TypeDef& oType, const GPIOPuPd_TypeDef& ppType, const GPIOSpeed_TypeDef& ioSpeed )
{
	enablePortRCC(mPort);
	InitStruct.GPIO_Pin = 1<<mPinBit;
	InitStruct.GPIO_Mode = ioMode;
	InitStruct.GPIO_Speed = ioSpeed;
	InitStruct.GPIO_OType = oType;
	InitStruct.GPIO_PuPd = ppType;
	GPIO_Init( mPort, &InitStruct );
}

void QIO::enablePortRCC( GPIO_TypeDef* const &GPIOx)
{
		 if( GPIOx == GPIOA ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE ); }
	else if( GPIOx == GPIOB ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE ); }
	else if( GPIOx == GPIOC ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE ); }
	else if( GPIOx == GPIOD ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE ); }
	else if( GPIOx == GPIOE ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE ); }
	else if( GPIOx == GPIOF ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF, ENABLE ); }
	else if( GPIOx == GPIOG ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOG, ENABLE ); }
	else if( GPIOx == GPIOH ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOH, ENABLE ); }
	else if( GPIOx == GPIOI ) { RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOI, ENABLE ); }
}
