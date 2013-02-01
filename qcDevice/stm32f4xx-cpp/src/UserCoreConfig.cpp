#include "UserCoreConfig.hpp"
#include "QtuC_Tools.hpp"

void UserCoreConfig()
{
	// Init led pins
	RCC_AHB1PeriphClockCmd( CoreCfg_Led_Debug_PortRCC | CoreCfg_Led_Resource_PortRCC, ENABLE );
	GPIO_InitTypeDef gi;
	gi.GPIO_Mode = GPIO_Mode_OUT;
	gi.GPIO_Speed = GPIO_Speed_100MHz;
	gi.GPIO_OType = GPIO_OType_PP;
	gi.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gi.GPIO_Pin = CoreCfg_Led_Resource_Pin;
	GPIO_Init( CoreCfg_Led_Resource_Port, &gi );
	gi.GPIO_Pin = CoreCfg_Led_Debug_Pin;
	GPIO_Init( CoreCfg_Led_Debug_Port, &gi );

	// Set ISR priority grouping
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_1 );
}

extern "C" void defaultInterruptHandler(void)
{
	// Light up defaultSVC LED
	RCC_AHB1PeriphClockCmd( CoreCfg_Led_DefaultISR_PortRCC, ENABLE );
	GPIO_InitTypeDef gi;
	gi.GPIO_Mode = GPIO_Mode_OUT;
	gi.GPIO_Speed = GPIO_Speed_100MHz;
	gi.GPIO_OType = GPIO_OType_PP;
	gi.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gi.GPIO_Pin = CoreCfg_Led_DefaultISR_Pin;
	GPIO_Init( CoreCfg_Led_DefaultISR_Port, &gi );
	CoreCfg_Led_DefaultISR_Port->BSRRL = CoreCfg_Led_DefaultISR_Pin;

	// also send a message
	QtuC::Tools::sendMessage( QtuC::msgWarning, "Device entered the default interrupt handler, looping infinitely..." );

	while(1);
}

extern "C" void defaultFaultHandler(void)
{
	// Light up Fault LED
	RCC_AHB1PeriphClockCmd( CoreCfg_Led_Fault_PortRCC, ENABLE );
	GPIO_InitTypeDef gi;
	gi.GPIO_Mode = GPIO_Mode_OUT;
	gi.GPIO_Speed = GPIO_Speed_100MHz;
	gi.GPIO_OType = GPIO_OType_PP;
	gi.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gi.GPIO_Pin = CoreCfg_Led_Fault_Pin;
	GPIO_Init( CoreCfg_Led_Fault_Port, &gi );
	CoreCfg_Led_Fault_Port->BSRRL = CoreCfg_Led_Fault_Pin;

	// also send a message
	QtuC::Tools::sendMessage( QtuC::msgError, "Device entered the default fault handler, looping infinitely..." );

	while(1);
}
