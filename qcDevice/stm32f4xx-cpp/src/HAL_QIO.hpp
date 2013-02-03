#ifndef HAL_QIO_H
#define HAL_QIO_H

#include "stm32f4xx_gpio.h"

/// @name Define pin bits
/// @{
#define GPIO_Pin_0_Bit  ((uint8_t)0)
#define GPIO_Pin_1_Bit  ((uint8_t)1)
#define GPIO_Pin_2_Bit  ((uint8_t)2)
#define GPIO_Pin_3_Bit  ((uint8_t)3)
#define GPIO_Pin_4_Bit  ((uint8_t)4)
#define GPIO_Pin_5_Bit  ((uint8_t)5)
#define GPIO_Pin_6_Bit  ((uint8_t)6)
#define GPIO_Pin_7_Bit  ((uint8_t)7)
#define GPIO_Pin_8_Bit  ((uint8_t)8)
#define GPIO_Pin_9_Bit  ((uint8_t)9)
#define GPIO_Pin_10_Bit ((uint8_t)10)
#define GPIO_Pin_11_Bit ((uint8_t)11)
#define GPIO_Pin_12_Bit ((uint8_t)12)
#define GPIO_Pin_13_Bit ((uint8_t)13)
#define GPIO_Pin_14_Bit ((uint8_t)14)
#define GPIO_Pin_15_Bit ((uint8_t)15)
/// @}

namespace HAL
{

/** Quick IO handling helper class.
 *	For a quick initialization, you can use the static init() method.
 *	To instantiate a QIO object, use the constructor.*/
class QIO
{
public:
	QIO( GPIO_TypeDef* const &ioPort, uint8_t const &ioPinBit );

	/// Initialize pin.
	static void init( GPIO_TypeDef* const &GPIOx, const uint32_t &pin, const GPIOMode_TypeDef &ioMode, const GPIOOType_TypeDef &oType = GPIO_OType_PP, const GPIOPuPd_TypeDef &ppType = GPIO_PuPd_NOPULL, const GPIOSpeed_TypeDef &ioSpeed = GPIO_Speed_100MHz, bool initClock = true );
	//static inline void init( GPIO_TypeDef* const &GPIOx, const uint8_t &pinBit, const GPIOMode_TypeDef &ioMode, const GPIOOType_TypeDef &oType = GPIO_OType_PP, const GPIOPuPd_TypeDef &ppType = GPIO_PuPd_NOPULL, const GPIOSpeed_TypeDef &ioSpeed = GPIO_Speed_100MHz, bool initClock = true )
	//	{ init( GPIOx, (uint32_t)(1<<pinBit), ioMode, oType, ppType, ioSpeed, initClock ); }

	void init( GPIOMode_TypeDef const &ioMode, const GPIOOType_TypeDef &oType = GPIO_OType_PP, const GPIOPuPd_TypeDef &ppType = GPIO_PuPd_NOPULL, const GPIOSpeed_TypeDef &ioSpeed = GPIO_Speed_100MHz );

	inline void set()
		{ mPort->BSRRL = mPin; }
	inline void reset()
		{ mPort->BSRRH = mPin; }

	static GPIO_InitTypeDef InitStruct;

	static void enablePortRCC( GPIO_TypeDef* const &GPIOx );

	inline GPIO_TypeDef* const port() const
		{ return mPort; }

	inline uint8_t const pinBit() const
		{ return mPinBit; }

	inline uint32_t const pin() const
		{ return mPin; }

private:
	GPIO_TypeDef* mPort;
	uint8_t mPinBit;
	uint32_t mPin;

};

}	//HAL::
#endif // HAL_QIO_H
