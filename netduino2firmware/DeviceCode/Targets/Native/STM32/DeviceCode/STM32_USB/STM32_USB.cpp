#include <tinyhal.h>
//#include <pal\com\usb\USB.h>
#include "STM32_USB.h"
#if defined(PLATFORM_ARM_STM32F4_ANY)
#include "..\stm32f4xx.h"
#elif defined(PLATFORM_ARM_STM32F2_ANY)
#include "..\stm32f2xx.h"
#endif

USB_CONTROLLER_STATE UsbControllerState[1];     // Only 1 USB Client Controller for this device

USB_CONTROLLER_STATE * STM32_USB_Driver::GetState( int Controller )
{
    if( Controller != 0 )       // There is only one USB device controller for this device
        return NULL;

    return &UsbControllerState[0];
}

HRESULT STM32_USB_Driver::Initialize( int Controller )
{
	// PA9  VBUS
	// PA11 DM
	// PA12 DP
	// PA10 ID

	// Configure port A pins 8, 9, 10, 11, 12 to Alternate function 10
	//GPIOA->AFR[0] = 0x00000000;
	GPIOA->AFR[1] |= 0x000AAAAA;

	// TODO: Configure port A pins 8, 9, 11, 12 to alternate function, 100 Mhz, no pull up/pull down and Push/Pull,
        GPIOA->MODER |= 0x00A28000;
        GPIOA->OTYPER |= 0x00000000;
        GPIOA->OSPEEDR |= 0x00F3C000;
        GPIOA->PUPDR |= 0x00000000;
        
	// TODO: Configure port A pin 10 to alternate function, 100 Mhz, pull up and open drain
        GPIOA->MODER |= 0x00080000;
        GPIOA->OTYPER |= 0x00000200;
        GPIOA->OSPEEDR |= 0x000C0000;
        GPIOA->PUPDR |= 0x00040000;
        
	// Enable USB peripheral clock.
	RCC->AHB1ENR |= RCC_AHB2Periph_OTG_FS;
}

HRESULT STM32_USB_Driver::Uninitialize( int Controller )
{
	GPIOA->AFR[1] &= 0xFFF00000;

	// Disable USB peripheral clock
	RCC->AHB1ENR &= ~RCC_AHB2Periph_OTG_FS;
}

BOOL STM32_USB_Driver::StartOutput( USB_CONTROLLER_STATE* State, int endpoint )
{
}

BOOL STM32_USB_Driver::RxEnable( USB_CONTROLLER_STATE *State, int endpoint )
{
}

BOOL STM32_USB_Driver::GetInterruptState()
{
}

BOOL STM32_USB_Driver::ProtectPins( int Controller, BOOL On )
{
}