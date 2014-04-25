////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for STM32: Copyright (c) Oberon microsystems, Inc.
//  Portions Copyright (c) Secret Labs LLC and the Netduino community. All rights reserved.
//
//  *** GPIO Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include <cores\arm\include\cpu.h>
#include "STM32_GPIO_functions.h";
#include "..\STM32_RCC\STM32_RCC_functions.h"
#if defined(PLATFORM_ARM_STM32F4_ANY)
#include "..\stm32f4xx.h"
#elif defined(PLATFORM_ARM_STM32F2_ANY)
#include "..\stm32f2xx.h"
#else
#include "..\stm32f10x.h"
#endif

#if defined(STM32_GPIO_USER_LED)
static const UINT8 g_STM32_GPIO_USER_LED = STM32_GPIO_USER_LED;
#endif
#if defined(STM32_GPIO_RESET_BTN)
static const UINT8 g_STM32_GPIO_RESET_BTN = STM32_GPIO_RESET_BTN;
#endif

//--//

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
#define STM32_Gpio_MaxPorts 9  // A..I
#else
#define STM32_Gpio_MaxPorts 7  // A..G
#endif
#define STM32_Gpio_MaxPins (STM32_Gpio_MaxPorts * 16)
#define STM32_Gpio_MaxInt 16

//--//

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "c_Gpio_Attributes_STM32"
#endif

const UINT8 __section(rodata) c_Gpio_Attributes[STM32_Gpio_MaxPins] =
{
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   0   (port a)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   1
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   2
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   3
#if defined(PLATFORM_ARM_NetduinoPlus2)
    GPIO_ATTRIBUTE_INPUT			, //   4 // special case: ENC_INT on Netduino Plus 2
#else
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   4
#endif
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   5
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   6
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   7
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   8
#if defined(PLATFORM_ARM_Netduino2) || defined(PLATFORM_ARM_NetduinoPlus2) || defined(PLATFORM_ARM_NetduinoGo)
    GPIO_ATTRIBUTE_INPUT			, //   9 // special case: USB_FS_VBUS on Netduino 2/Netduino Plus 2/Netduino Go
#else
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //   9
#endif
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  10
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  11
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  12
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  13
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  14
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  15
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  16   (port b)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  17
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  18
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  19
#if defined(PLATFORM_ARM_NetduinoPlus2)
    GPIO_ATTRIBUTE_INPUT			, //  20 // special case: ENC_MISO on Netduino Plus 2
#else
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  20
#endif
#if defined(PLATFORM_ARM_NetduinoPlus2)
    GPIO_ATTRIBUTE_INPUT			, //  21 // special case: /MICRO_SD_CARD_INSERTED on Netduino Plus 2
#else
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  21
#endif
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  22
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  23
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  24
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  25
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  26
#if defined(PLATFORM_ARM_Netduino2) || defined(PLATFORM_ARM_NetduinoPlus2) || defined(PLATFORM_ARM_NetduinoShieldBase)
    GPIO_ATTRIBUTE_INPUT			, //  27 // special case: SWITCH1 on Netduino 2/Netduino Plus 2; /RESET_SHIELD on Shield Base
#else
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  27
#endif
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  28
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  29
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  30
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  31
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  32   (port c)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  33
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  34
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  35
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  36
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  37
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  38
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  39
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  40
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  41
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  42
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  43
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  44
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  45
#if defined(PLATFORM_ARM_Netduino2) || defined(PLATFORM_ARM_NetduinoPlus2) || defined(PLATFORM_ARM_NetduinoGo)	
    GPIO_ATTRIBUTE_INPUT			, //  46 // special case: SWITCH1 on Netduino 2/Netduino Plus 2/Netduino Go
#else
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  46
#endif
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  47
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  48   (port d)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  49
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  50
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  51
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  52
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  53
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  54
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  55
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  56
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  57
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  58
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  59
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  60
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  61
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  62
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  63
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  64   (port e)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  65
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  66
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  67
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  68
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  69
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  70
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  71
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  72
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  73
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  74
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  75
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  76
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  77
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  78
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  79
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  80   (port f)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  81
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  82
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  83
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  84
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  85
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  86
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  87
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  88
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  89
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  90
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  91
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  92
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  93
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  94
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  95
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  96   (port g)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  97
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  98
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  99
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  100
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  101
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  102
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  103
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  104
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  105
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  106
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  107
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  108
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  109
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  110
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  111
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  112  (port h)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  113
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  114
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  115
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  116
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  117
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  118
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  119
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  120
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  121
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  122
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  123
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  124
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  125
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  126
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  127	
#if (STM32_Gpio_MaxPorts > 8)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  128  (port i)
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  129
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  130
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  131
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  132
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  133
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  134
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  135
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  136
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  137
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  138
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  139
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  140
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  141
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  142
    GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT, //  143	
#endif
};

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata
#endif

#endif
//--//

#define STM32_GPIO_ALT_MODE_OUT    GPIO_ALT_MODE_1
#define STM32_GPIO_ALT_MODE_HIGH   GPIO_ALT_MODE_2
#define STM32_GPIO_ALT_MODE_OD     GPIO_ALT_MODE_3
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
#define STM32_GPIO_ALT_MODE_ANALOG GPIO_ALT_MODE_4
#define STM32_GPIO_ALT_MODE_IN	   GPIO_ALT_MODE_5
#endif

// indexed port configuration access
#define Port(port) ((GPIO_TypeDef *) (GPIOA_BASE + (port << 10)))

static UINT32 g_debounceTicks;
static UINT16 g_pinReserved[STM32_Gpio_MaxPorts]; //  1 bit per pin

static GPIO_INTERRUPT_SERVICE_ROUTINE g_ISR[STM32_Gpio_MaxInt]; // interrupt handlers
static void* g_ISR_Param[STM32_Gpio_MaxInt]; // interrupt handler parameters

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
void STM32_GPIO_AFConfig(UINT32 pin, UINT8 GPIO_AF)
{
	UINT32 num = (pin & 0x0F);
	UINT32 shift = (pin & 0x07) * 4; // 4 bits/pin
        UINT32 mask = 0x0F << shift;
	UINT32 alternateFunction = ((UINT32)GPIO_AF & 0x0F) << shift;
	GPIO_TypeDef* port = Port(pin >> 4); // pointer to the actual port registers
	
	port->AFR[num >> 3] = (port->AFR[num >> 3] & ~mask) | alternateFunction;
}
#endif

#if defined(STM32_GPIO_RESET_BTN)
void ResetButtonIsrProcedure( GPIO_PIN pin, BOOL pinState, void* context )
{
	// turn on blue LED(s)
#if defined(STM32_GPIO_USER_LED)
    CPU_GPIO_DisablePin(g_STM32_GPIO_USER_LED, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
	CPU_GPIO_SetPinState(g_STM32_GPIO_USER_LED, TRUE);
#endif
#if defined(PLATFORM_ARM_NetduinoPlus2)
	// also Ethernet chip in reset mode
    CPU_GPIO_EnableOutputPin(0x32, FALSE); // assert /ENC_RESET
#endif
#if defined(PLATFORM_ARM_NetduinoGo)
	// TODO: revisit this, using a DEFINE set and maybe disabling any PWM on these pins first, if necessary
    CPU_GPIO_DisablePin(22, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
    CPU_GPIO_DisablePin(23, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
    CPU_GPIO_DisablePin(24, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
    CPU_GPIO_DisablePin(25, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
    CPU_GPIO_DisablePin(38, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
    CPU_GPIO_DisablePin(39, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
    CPU_GPIO_DisablePin(40, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
    CPU_GPIO_DisablePin(41, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY);
        CPU_GPIO_EnableOutputPin(22, TRUE); // LED_GOPORT1
        CPU_GPIO_EnableOutputPin(23, TRUE); // LED_GOPORT2
        CPU_GPIO_EnableOutputPin(24, TRUE); // LED_GOPORT3
        CPU_GPIO_EnableOutputPin(25, TRUE); // LED_GOPORT4
        CPU_GPIO_EnableOutputPin(38, TRUE); // LED_GOPORT5
        CPU_GPIO_EnableOutputPin(39, TRUE); // LED_GOPORT6
        CPU_GPIO_EnableOutputPin(40, TRUE); // LED_GOPORT7
        CPU_GPIO_EnableOutputPin(41, TRUE); // LED_GOPORT8
#endif
    // disable USB
    CPU_USB_Uninitialize(0);
    // wait for button to be released
	while (CPU_GPIO_GetPinState(g_STM32_GPIO_RESET_BTN)) ;
    // reset mcu
	CPU_Reset();
}
#endif

/*
 * Interrupt Handler
 */

void STM32_GPIO_ISR (int num)  // 0 <= num <= 15
{
    INTERRUPT_START

    GPIO_INTERRUPT_SERVICE_ROUTINE isr = g_ISR[num];
    if (isr) {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        int port = (SYSCFG->EXTICR[num >> 2] >> ((num & 0x3) << 2)) & 0xF;
#else
        int port = (AFIO->EXTICR[num >> 2] >> ((num & 0x3) << 2)) & 0xF;
#endif
        isr((port << 4) | num, (Port(port)->IDR >> num) & 1, g_ISR_Param[num]);
    }

    INTERRUPT_END
}

void STM32_GPIO_Interrupt0 (void* param) // EXTI0
{
    EXTI->PR = 1 << 0;   // reset pending bit
    STM32_GPIO_ISR(0);
}

void STM32_GPIO_Interrupt1 (void* param) // EXTI1
{
    EXTI->PR = 1 << 1;   // reset pending bit
    STM32_GPIO_ISR(1);
}

void STM32_GPIO_Interrupt2 (void* param) // EXTI2
{
    EXTI->PR = 1 << 2;   // reset pending bit
    STM32_GPIO_ISR(2);
}

void STM32_GPIO_Interrupt3 (void* param) // EXTI3
{
    EXTI->PR = 1 << 3;   // reset pending bit
    STM32_GPIO_ISR(3);
}

void STM32_GPIO_Interrupt4 (void* param) // EXTI4
{
    EXTI->PR = 1 << 4;   // reset pending bit
    STM32_GPIO_ISR(4);
}

void STM32_GPIO_Interrupt5 (void* param) // EXTI5 - EXTI9
{
    UINT32 pending = EXTI->PR & 0x03E0; // pending bits 5..9
    EXTI->PR = pending;    // reset pending bits 5..9
    pending &= EXTI->IMR;  // remove masked bits
    int num = 5; pending >>= 5;
    do {
        if (pending & 1) STM32_GPIO_ISR(num);
        num++; pending >>= 1;
    } while (pending);
}

void STM32_GPIO_Interrupt10 (void* param) // EXTI10 - EXTI15
{
    UINT32 pending = EXTI->PR & 0xFC00; // pending bits 10..15
    EXTI->PR = pending;    // reset pending bits 10..15
    pending &= EXTI->IMR;  // remove masked bits
    int num = 10; pending >>= 10;
    do {
        if (pending & 1) STM32_GPIO_ISR(num);
        num++; pending >>= 1;
    } while (pending);
}

BOOL STM32_GPIO_Set_Interrupt( UINT32 pin, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, void* ISR_Param, GPIO_INT_EDGE mode)
{
    UINT32 num = pin & 0x0F;
    UINT32 bit = 1 << num;
    UINT32 shift = (num & 0x3) << 2; // 4 bit fields
    UINT32 idx = num >> 2;
    UINT32 mask = 0xF << shift;
    UINT32 config = (pin >> 4) << shift; // port number configuration
    
    GLOBAL_LOCK(irq);

    if (ISR) {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        if ((SYSCFG->EXTICR[idx] & mask) != config) {
#else
        if ((AFIO->EXTICR[idx] & mask) != config) {
#endif
            if (EXTI->IMR & bit) return FALSE; // interrupt in use
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
            SYSCFG->EXTICR[idx] = SYSCFG->EXTICR[idx] & ~mask | config;
#else
            AFIO->EXTICR[idx] = AFIO->EXTICR[idx] & ~mask | config;
#endif
        }
        g_ISR[num] = ISR;
        g_ISR_Param[num] = ISR_Param;
        if (mode == GPIO_INT_EDGE_HIGH || mode == GPIO_INT_EDGE_BOTH) {
            EXTI->RTSR |= bit;
        } else {
            EXTI->RTSR &= ~bit;
        }
        if (mode == GPIO_INT_EDGE_LOW || mode == GPIO_INT_EDGE_BOTH) {
            EXTI->FTSR |= bit;
        } else {
            EXTI->FTSR &= ~bit;
        }
        EXTI->PR = bit; // remove pending interrupt
        EXTI->IMR |= bit; // enable interrupt
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    } else if ((SYSCFG->EXTICR[idx] & mask) == config) {
#else
    } else if ((AFIO->EXTICR[idx] & mask) == config) {
#endif
        EXTI->IMR &= ~bit; // disable interrupt
        g_ISR[num] = NULL;
    }
    return TRUE;
}

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
void STM32_GPIO_Pin_Config( GPIO_PIN pin, UINT32 mode, UINT32 type, UINT32 speed, GPIO_RESISTOR resistor )
#else
void STM32_GPIO_Pin_Config( GPIO_PIN pin, UINT32 config )
#endif
{
	GPIO_TypeDef* port = Port(pin >> 4); // pointer to the actual port registers
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	UINT32 doubleBitShift = (pin & 0x0F) << 1; // 2 bits / pin
	UINT32 singleBitShift = (pin & 0x0F) << 0; // 1 bit / pin
	mode <<= doubleBitShift;
	type <<= singleBitShift;
	speed <<= doubleBitShift;
	UINT32 doubleBitMask = 0x03 << doubleBitShift;
	UINT32 singleBitMask = 0x01 << singleBitShift;
#else
    UINT32 shift = (pin & 7) << 2; // 4 bits / pin
    config <<= shift;
    UINT32 mask = 0xF << shift;
#endif
    
    GLOBAL_LOCK(irq);

#if defined(STM32_GPIO_RESET_BTN)
	// make sure that the pushbutton resistor is always disabled
	if (pin == STM32_GPIO_RESET_BTN)
	{
		resistor == RESISTOR_DISABLED;
	}
#endif

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    port->MODER = (port->MODER & ~doubleBitMask) | mode;
	port->OTYPER = (port->OTYPER & ~singleBitMask) | type;
    port->OSPEEDR = (port->OSPEEDR & ~doubleBitMask) | speed;

    if(resistor == RESISTOR_PULLUP)
    {
		port->PUPDR = (port->PUPDR & ~doubleBitMask) | (0x01 << doubleBitShift);
    }
    else if(resistor == RESISTOR_PULLDOWN)
    {
		port->PUPDR = (port->PUPDR & ~doubleBitMask) | (0x02 << doubleBitShift);
    }
    else
    {
		port->PUPDR = (port->PUPDR & ~doubleBitMask) | (0x00 << doubleBitShift);
    }
#else
    if (pin & 0x08) { // bit 8 - 15
        port->CRH = port->CRH & ~mask | config;
    } else { // bit 0-7
        port->CRL = port->CRL & ~mask | config;
    }
#endif
}



BOOL CPU_GPIO_Initialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();

    CPU_GPIO_SetDebounce(20); // ???

    for (int i = 0; i < STM32_Gpio_MaxPorts; i++) {
        g_pinReserved[i] = 0;
    }

    EXTI->IMR = 0; // disable all external interrupts;
    CPU_INTC_ActivateInterrupt(EXTI0_IRQn, STM32_GPIO_Interrupt0, 0);
    CPU_INTC_ActivateInterrupt(EXTI1_IRQn, STM32_GPIO_Interrupt1, 0);
    CPU_INTC_ActivateInterrupt(EXTI2_IRQn, STM32_GPIO_Interrupt2, 0);
    CPU_INTC_ActivateInterrupt(EXTI3_IRQn, STM32_GPIO_Interrupt3, 0);
    CPU_INTC_ActivateInterrupt(EXTI4_IRQn, STM32_GPIO_Interrupt4, 0);
    CPU_INTC_ActivateInterrupt(EXTI9_5_IRQn, STM32_GPIO_Interrupt5, 0);
    CPU_INTC_ActivateInterrupt(EXTI15_10_IRQn, STM32_GPIO_Interrupt10, 0);

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    STM32_RCC_APB2PeripheralClockEnable(RCC_APB2ENR_SYSCFGEN); /* ENABLE SYSCFG CLOCK TO ENABLE PIN INTERRUPT ASSIGNMENTS */
#endif

    return TRUE;
}

BOOL CPU_GPIO_Uninitialize()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();

    EXTI->IMR = 0; // disable all external interrupts;
    CPU_INTC_DeactivateInterrupt(EXTI0_IRQn);
    CPU_INTC_DeactivateInterrupt(EXTI1_IRQn);
    CPU_INTC_DeactivateInterrupt(EXTI2_IRQn);
    CPU_INTC_DeactivateInterrupt(EXTI3_IRQn);
    CPU_INTC_DeactivateInterrupt(EXTI4_IRQn);
    CPU_INTC_DeactivateInterrupt(EXTI9_5_IRQn);
    CPU_INTC_DeactivateInterrupt(EXTI15_10_IRQn);

    return TRUE;
}

UINT32 CPU_GPIO_Attributes( GPIO_PIN pin )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if(pin < STM32_Gpio_MaxPins) {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        return c_Gpio_Attributes[pin];
#else
        return GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT;
#endif
    }
    return GPIO_ATTRIBUTE_NONE;
}

void CPU_GPIO_DisablePin( GPIO_PIN pin, GPIO_RESISTOR resistor, UINT32 output, GPIO_ALT_MODE alternate )
{
	NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
	if (pin < STM32_Gpio_MaxPins) {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
		UINT32 mode;
		UINT32 type;
		UINT32 speed;
#else
        UINT32 config;
#endif

		if (alternate == STM32_GPIO_ALT_MODE_OD) 			// alternate open drain
		{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
			mode = 2;
			type = 1;
			speed = 0; // 2 MHz
#else
			config = 0xD;
#endif
		}
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
		else if (alternate == STM32_GPIO_ALT_MODE_ANALOG) 
		{
			mode = 3;
			type = 0;
			speed = 0; // 2 MHz
		}
#endif
		else if (output) 
		{
			if (alternate == STM32_GPIO_ALT_MODE_OUT)
			{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
				mode = 2;
				type = 0;
				speed = 1; // 25 MHz
#else
				config = 0x9;
#endif
			} 
			else if (alternate == STM32_GPIO_ALT_MODE_HIGH) 
			{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
				mode = 2;
				type = 0;
				speed = 3; // 100 MHz
#else
				config = 0xB;
#endif
			} 
			else 
			{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
				mode = 1; // standard GPIO output
				type = 0;
				speed = 1; // 25 MHz
#else
				config = 0x1;
#endif
			}
		} 
		else
		{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
			if (alternate == STM32_GPIO_ALT_MODE_IN)
			{
				mode = 2; // alternate function input
				type = 0;
				speed = 0;
			}
			else
			{
				mode = 0; // input
				type = 0;
				speed = 0;
			}
#else
            if (alternate) config = 0;                                    // analog mode
            else if (resistor) config = 0x8;                              // pull up/down
            else config = 0x4;                                            // general purpose input
#endif
		}
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
#else
        if (resistor) {
            CPU_GPIO_SetPinState(pin, resistor == RESISTOR_PULLUP); // pull up or down
        }
#endif
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        STM32_GPIO_Pin_Config(pin, mode, type, speed, resistor);
#else
        STM32_GPIO_Pin_Config(pin, config);
#endif
        STM32_GPIO_Set_Interrupt(pin, NULL, 0, GPIO_INT_NONE); // disable interrupt
     }
}

void CPU_GPIO_EnableOutputPin( GPIO_PIN pin, BOOL initialState )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (pin < STM32_Gpio_MaxPins) {
        CPU_GPIO_SetPinState(pin, initialState);
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        STM32_GPIO_Pin_Config(pin, 0x1, 0x0, 0x1, RESISTOR_DISABLED); // general purpose output (25MHz)
#else
        STM32_GPIO_Pin_Config(pin, 0x1); // general purpose output
#endif
        STM32_GPIO_Set_Interrupt(pin, NULL, 0, GPIO_INT_NONE); // disable interrupt
    }
}

BOOL CPU_GPIO_EnableInputPin( GPIO_PIN pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, GPIO_INT_EDGE edge, GPIO_RESISTOR resistor )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return CPU_GPIO_EnableInputPin2( pin, GlitchFilterEnable, ISR, 0, edge, resistor );
}

BOOL CPU_GPIO_EnableInputPin2( GPIO_PIN pin, BOOL GlitchFilterEnable, GPIO_INTERRUPT_SERVICE_ROUTINE ISR, void* ISR_Param, GPIO_INT_EDGE edge, GPIO_RESISTOR resistor )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (pin >= STM32_Gpio_MaxPins) return FALSE;

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    UINT32 mode = 0x0; // general purpose input
    UINT32 type = 0x0;
    UINT32 speed = 0x0;
#else
    UINT32 config = 0x4; // general purpose input
#endif
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
#else
    if (resistor) {
        config = 0x8;    // pull up/down
        CPU_GPIO_SetPinState(pin, resistor == RESISTOR_PULLUP);
    }
#endif
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    STM32_GPIO_Pin_Config(pin, mode, type, speed, resistor);
#else
    STM32_GPIO_Pin_Config(pin, config);
#endif

    if (edge > GPIO_INT_EDGE_BOTH) return FALSE;
    return STM32_GPIO_Set_Interrupt(pin, ISR, ISR_Param, edge);
}

BOOL CPU_GPIO_GetPinState( GPIO_PIN pin )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (pin >= STM32_Gpio_MaxPins) return FALSE;

    GPIO_TypeDef* port = Port(pin >> 4); // pointer to the actual port registers
    return (port->IDR >> (pin & 0xF)) & 1;
}

void CPU_GPIO_SetPinState( GPIO_PIN pin, BOOL pinState )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (pin < STM32_Gpio_MaxPins) {
        GPIO_TypeDef* port = Port(pin >> 4); // pointer to the actual port registers
        UINT32 bit = 1 << (pin & 0x0F);
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        if(!pinState) 
		{
    	    port->BSRRH = bit;
		} 
		else 
		{
    	    port->BSRRL = bit;
		}
#else
        if (!pinState) bit <<= 16; // reset bits
        port->BSRR = bit;
#endif
    }
}

//--//

BOOL CPU_GPIO_PinIsBusy( GPIO_PIN pin )  // busy == reserved
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (pin >= STM32_Gpio_MaxPins) return FALSE;
    int port = pin >> 4, sh = pin & 0x0F;
    return (g_pinReserved[port] >> sh) & 1;
}

BOOL CPU_GPIO_ReservePin( GPIO_PIN pin, BOOL fReserve )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (pin >= STM32_Gpio_MaxPins) return FALSE;
    int port = pin >> 4, bit = 1 << (pin & 0x0F);
    GLOBAL_LOCK(irq);
    if (fReserve)
	{ 
		if (g_pinReserved[port] & bit)
			return FALSE; // already reserved
		g_pinReserved[port] |= bit;
                
#if defined(STM32_GPIO_RESET_BTN)
                if (pin == g_STM32_GPIO_RESET_BTN)
		{
                  STM32_GPIO_Set_Interrupt(pin, NULL, 0, GPIO_INT_NONE); // disable BTN interrupt (i.e. disable as reset button)
		}
#endif
	}
	else
	{
		g_pinReserved[port] &= ~bit;
#if defined(STM32_GPIO_RESET_BTN)
                if (pin == g_STM32_GPIO_RESET_BTN)
		{
			CPU_GPIO_EnableInputPin(pin, TRUE, ResetButtonIsrProcedure, GPIO_INT_EDGE_HIGH, RESISTOR_DISABLED); // enable BTN interrupt (i.e. enable as reset button)
		}
#endif
	}
	
    return TRUE;
}

UINT32 CPU_GPIO_GetDebounce()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return CPU_TicksToTime(g_debounceTicks) / 10000; // ticks -> 100ns -> ms
}

BOOL CPU_GPIO_SetDebounce( INT64 debounceTimeMilliseconds )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    if (debounceTimeMilliseconds > 0 && debounceTimeMilliseconds < 10000) {
        g_debounceTicks = CPU_MillisecondsToTicks((UINT32)debounceTimeMilliseconds);
        return TRUE;
    }
    return FALSE;
}

INT32 CPU_GPIO_GetPinCount()
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return STM32_Gpio_MaxPins;
}

void CPU_GPIO_GetPinsMap( UINT8* pins, size_t size )
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    const UINT8*    src = c_Gpio_Attributes;
    UINT8* dst = pins;   
    UINT32 maxpin = STM32_Gpio_MaxPins;

    if ( size ==0 ) return;
    while ((size--) && (maxpin --))
    {
        *dst = *src;  
        ++dst; ++src;
    }
#else
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    for (int i = 0; i < size && i < STM32_Gpio_MaxPins; i++) {
         pins[i] = GPIO_ATTRIBUTE_INPUT | GPIO_ATTRIBUTE_OUTPUT;
    }
#endif
}

UINT8 CPU_GPIO_GetSupportedResistorModes( GPIO_PIN pin )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return (1 << RESISTOR_DISABLED) | (1 << RESISTOR_PULLUP) | (1 << RESISTOR_PULLDOWN);
}

UINT8 CPU_GPIO_GetSupportedInterruptModes( GPIO_PIN pin )
{
    NATIVE_PROFILE_HAL_PROCESSOR_GPIO();
    return (1 << GPIO_INT_EDGE_LOW) | (1 << GPIO_INT_EDGE_HIGH ) | (1 << GPIO_INT_EDGE_BOTH);
}
