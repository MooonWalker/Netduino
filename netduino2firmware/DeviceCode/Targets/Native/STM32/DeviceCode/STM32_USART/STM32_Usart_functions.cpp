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
//  *** Serial Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cores\arm\include\cpu.h>
#include "..\STM32_GPIO\STM32_GPIO_functions.h";
#if defined(PLATFORM_ARM_STM32F4_ANY)
#include "..\stm32f4xx.h"
#elif defined(PLATFORM_ARM_STM32F2_ANY)
#include "..\stm32f2xx.h"
#else
#include "..\stm32f10x.h"
#endif

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
static const UINT8 g_STM32_UART_UartNum[] = STM32_UART_NUM;
static const UINT8 g_STM32_UART_TxPin[] = STM32_UART_TX_PIN;
static const UINT8 g_STM32_UART_RxPin[] = STM32_UART_RX_PIN;
static const UINT8 g_STM32_UART_RtsPin[] = STM32_UART_RTS_PIN;
static const UINT8 g_STM32_UART_CtsPin[] = STM32_UART_CTS_PIN;
//#define STM32_UART_PORTS ARRAYSIZE_CONST_EXPR(g_STM32_UART_TxPin) // number of ports

// IO addresses
//static USART_TypeDef* g_STM32_Uart[] = {USART1, USART2, USART3, UART4, UART5, USART6};
static USART_TypeDef* g_STM32_Uart[] = STM32_UART;
   
// Timer Alternate Functions
static const UINT8 g_STM32_UART_AlternateFunction[] = {7,7,7,8,8,8};
// Timer IRQs
static const UINT32 g_STM32_UART_Irq[] = {USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};
#else
static USART_TypeDef* g_STM32_Uart[] = {USART1, USART2, USART3}; // IO addresses
#endif


void STM32_USART_Handle_RX_IRQ (int ComPortNum, USART_TypeDef* uart)
{
    INTERRUPT_START;

    char c = (char)(uart->DR); // read RX data
    USART_AddCharToRxBuffer(ComPortNum, c);
    Events_Set(SYSTEM_EVENT_FLAG_COM_IN);

    INTERRUPT_END;
}

void STM32_USART_Handle_TX_IRQ (int ComPortNum, USART_TypeDef* uart)
{
    INTERRUPT_START;

    char c;
    if (USART_RemoveCharFromTxBuffer(ComPortNum, c)) {
        uart->DR = c;  // write TX data
    } else {
        uart->CR1 &= ~USART_CR1_TXEIE; // TX int disable
    }
    Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);

    INTERRUPT_END;
}

void STM32_USART_Interrupt0(void* param)
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    USART_TypeDef* uart = g_STM32_Uart[0];
    UINT16 sr = uart->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(0, uart);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(0, uart);
#else
    UINT16 sr = USART1->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(0, USART1);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(0, USART1);
#endif
}

void STM32_USART_Interrupt1(void* param)
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    USART_TypeDef* uart = g_STM32_Uart[1];
    UINT16 sr = uart->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(1, uart);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(1, uart);
#else
    UINT16 sr = USART2->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(1, USART2);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(1, USART2);
#endif
}

void STM32_USART_Interrupt2(void* param)
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    USART_TypeDef* uart = g_STM32_Uart[2];
    UINT16 sr = uart->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(2, uart);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(2, uart);
#else
    UINT16 sr = USART3->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(2, USART3);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(2, USART3);
#endif
}

void STM32_USART_Interrupt3(void* param)
{
    USART_TypeDef* uart = g_STM32_Uart[3];
    UINT16 sr = uart->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(3, uart);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(3, uart);
}

void STM32_USART_Interrupt4(void* param)
{
    USART_TypeDef* uart = g_STM32_Uart[4];
    UINT16 sr = uart->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(4, uart);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(4, uart);
}

void STM32_USART_Interrupt5(void* param)
{
    USART_TypeDef* uart = g_STM32_Uart[5];
    UINT16 sr = uart->SR;
    if (sr & USART_SR_RXNE) STM32_USART_Handle_RX_IRQ(5, uart);
    if (sr & USART_SR_TXE)  STM32_USART_Handle_TX_IRQ(5, uart);
}

BOOL CPU_USART_Initialize( int ComPortNum, int BaudRate, int Parity, int DataBits, int StopBits, int FlowValue )
{
    if (ComPortNum >= TOTAL_USART_PORT) return FALSE;
    if (Parity >= USART_PARITY_MARK) return FALSE;
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	if (DataBits < 7 || (DataBits == 7 && Parity == 0) || (DataBits == 9 && Parity) || DataBits > 9) return FALSE;
#endif
    
    GLOBAL_LOCK(irq);

    USART_TypeDef* uart = g_STM32_Uart[ComPortNum];
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	UINT32 uartNum = g_STM32_UART_UartNum[ComPortNum];
	UINT8 alternateFunc = g_STM32_UART_AlternateFunction[uartNum];
#endif
    UINT32 clk;

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    GPIO_PIN rxPin, txPin, ctsPin, rtsPin;
    CPU_USART_GetPins(ComPortNum, rxPin, txPin, ctsPin, rtsPin);

	if ((FlowValue & USART_FLOW_HW_OUT_EN) & ctsPin != 0xFF)
    	STM32_GPIO_AFConfig(ctsPin, alternateFunc);
	if ((FlowValue & USART_FLOW_HW_IN_EN) & rtsPin != 0xFF)
    	STM32_GPIO_AFConfig(rtsPin, alternateFunc);
#endif

    // enable UART clock
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    if(uart == USART1 || uart == USART6) { // USART1/USART6 on APB2
		UINT32 shift = (((UINT32)uart - USART1_BASE) >> 0x0A) + 1;
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN >> 1 << shift;
        clk = SYSTEM_APB2_CLOCK_HZ;
    } else { // USART2/USART3/UART4/UART5 on APB1
		UINT32 shift = (((UINT32)uart - USART2_BASE) >> 0x0A) + 1;
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN >> 1 << shift;
        clk = SYSTEM_APB1_CLOCK_HZ;
    }
#else
    if (ComPortNum) { // COM2/3 on APB1
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN >> 1 << ComPortNum;
        clk = SYSTEM_APB1_CLOCK_HZ;
    } else { // COM1 on APB2
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        clk = SYSTEM_APB2_CLOCK_HZ;
    }
#endif

    //  baudrate
    UINT16 div = (UINT16)((clk + (BaudRate >> 1)) / BaudRate); // rounded
    uart->BRR = div;

    // control
    UINT16 ctrl = USART_CR1_TE | USART_CR1_RE;
    if (DataBits == 9) ctrl |= USART_CR1_M;

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    if (Parity) {
		ctrl |= USART_CR1_PCE;
		if (DataBits == 8) ctrl |= USART_CR1_M;
	}
#else
    if (Parity) ctrl |= USART_CR1_PCE;
#endif
    if (Parity == USART_PARITY_ODD) ctrl |= USART_CR1_PS;
    uart->CR1 = ctrl;
    
    if (StopBits == USART_STOP_BITS_ONE) StopBits = 0;
    uart->CR2 = (UINT16)(StopBits << 12);

    ctrl = 0;
    if ((FlowValue & USART_FLOW_HW_OUT_EN) & ctsPin != 0xFF) ctrl |= USART_CR3_CTSE;
    if ((FlowValue & USART_FLOW_HW_IN_EN) & rtsPin != 0xFF)  ctrl |= USART_CR3_RTSE;
    uart->CR3 = ctrl;

    //CPU_GPIO_DisablePin(rxPin, RESISTOR_PULLUP, 0, GPIO_ALT_MODE_5); // should we pull up RX by default?
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    CPU_GPIO_DisablePin(rxPin, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_5); // alternate input
    STM32_GPIO_AFConfig(rxPin, alternateFunc);
#else
    GPIO_PIN rxPin, txPin, ctsPin, rtsPin;
    CPU_USART_GetPins(ComPortNum, rxPin, txPin, ctsPin, rtsPin);
    CPU_GPIO_DisablePin(rxPin, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY); // floating input
#endif
    CPU_GPIO_DisablePin(txPin, RESISTOR_DISABLED, 1, GPIO_ALT_MODE_1);  // alternate output
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    STM32_GPIO_AFConfig(txPin, alternateFunc);
#endif

    if (FlowValue & USART_FLOW_HW_OUT_EN)
	{
		if (ctsPin == 0xFF) return FALSE;
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    	CPU_GPIO_DisablePin(ctsPin, RESISTOR_DISABLED, 0, GPIO_ALT_MODE_5); // alternate input
	    STM32_GPIO_AFConfig(ctsPin, alternateFunc);
#else
        CPU_GPIO_DisablePin(ctsPin, RESISTOR_DISABLED, 0, GPIO_ALT_PRIMARY); // floating input
#endif
	}
    if (FlowValue & USART_FLOW_HW_IN_EN)
	{
		if (rtsPin == 0xFF) return FALSE;
    	CPU_GPIO_DisablePin(rtsPin, RESISTOR_DISABLED, 1, GPIO_ALT_MODE_1);  // alternate output
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	    STM32_GPIO_AFConfig(rtsPin, alternateFunc);
#endif
	}

    CPU_USART_ProtectPins(ComPortNum, FALSE);
    
    if (ComPortNum == 0) {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        CPU_INTC_ActivateInterrupt(g_STM32_UART_Irq[uartNum], STM32_USART_Interrupt0, 0);
#else
        CPU_INTC_ActivateInterrupt(USART1_IRQn, STM32_USART_Interrupt0, 0);
#endif
    } else if (ComPortNum == 1) {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        CPU_INTC_ActivateInterrupt(g_STM32_UART_Irq[uartNum], STM32_USART_Interrupt1, 0);
#else
        CPU_INTC_ActivateInterrupt(USART2_IRQn, STM32_USART_Interrupt1, 0);
#endif
    } else if (ComPortNum == 2) {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        CPU_INTC_ActivateInterrupt(g_STM32_UART_Irq[uartNum], STM32_USART_Interrupt2, 0);
#else
        CPU_INTC_ActivateInterrupt(USART3_IRQn, STM32_USART_Interrupt2, 0);
#endif
    } else if (ComPortNum == 3) {
        CPU_INTC_ActivateInterrupt(g_STM32_UART_Irq[uartNum], STM32_USART_Interrupt3, 0);
    } else if (ComPortNum == 4) {
        CPU_INTC_ActivateInterrupt(g_STM32_UART_Irq[uartNum], STM32_USART_Interrupt4, 0);
    } else if (ComPortNum == 5) {
        CPU_INTC_ActivateInterrupt(g_STM32_UART_Irq[uartNum], STM32_USART_Interrupt5, 0);
    }

    uart->CR1 |= USART_CR1_UE; // start uart

    return TRUE;
}

BOOL CPU_USART_Uninitialize( int ComPortNum )
{
    GLOBAL_LOCK(irq);
    
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    USART_TypeDef* uart = g_STM32_Uart[ComPortNum];
	UINT32 uartNum = g_STM32_UART_UartNum[ComPortNum];

    uart->CR1 = 0; // stop uart
    CPU_INTC_DeactivateInterrupt(g_STM32_UART_Irq[uartNum]);
#else
    g_STM32_Uart[ComPortNum]->CR1 = 0; // stop uart

    if (ComPortNum == 0) {
        CPU_INTC_DeactivateInterrupt(USART1_IRQn);
    } else if (ComPortNum == 1) {
        CPU_INTC_DeactivateInterrupt(USART2_IRQn);
    } else if (ComPortNum == 2) {
        CPU_INTC_DeactivateInterrupt(USART3_IRQn);
    }
#endif

    CPU_USART_ProtectPins(ComPortNum, TRUE);
    
    // disable UART clock
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    if(uart == USART1 || uart == USART6) { // USART1/USART6 on APB2
		UINT32 shift = (((UINT32)uart - USART1_BASE) >> 0x0A) + 1;
        RCC->APB2ENR &= ~(RCC_APB2ENR_USART1EN >> 1 << shift);
    } else { // USART2/USART3/UART4/UART5 on APB1
		UINT32 shift = (((UINT32)uart - USART2_BASE) >> 0x0A) + 1;
        RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN >> 1 << shift);
    }
#else
    if (ComPortNum) { // COM2 or COM3 on APB1
        RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN >> 1 << ComPortNum);
    } else { // COM1 on APB2
        RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
    }
#endif

    return TRUE;
}

BOOL CPU_USART_TxBufferEmpty( int ComPortNum )
{
    if (g_STM32_Uart[ComPortNum]->SR & USART_SR_TXE) return TRUE;
    return FALSE;
}

BOOL CPU_USART_TxShiftRegisterEmpty( int ComPortNum )
{
    if (g_STM32_Uart[ComPortNum]->SR & USART_SR_TC) return TRUE;
    return FALSE;
}

void CPU_USART_WriteCharToTxBuffer( int ComPortNum, UINT8 c )
{
#ifdef DEBUG
    ASSERT(CPU_USART_TxBufferEmpty(ComPortNum));
#endif
    g_STM32_Uart[ComPortNum]->DR = c;
}

void CPU_USART_TxBufferEmptyInterruptEnable( int ComPortNum, BOOL Enable )
{
    USART_TypeDef* uart = g_STM32_Uart[ComPortNum];
    if (Enable) {
        uart->CR1 |= USART_CR1_TXEIE;  // tx int enable
    } else {
        uart->CR1 &= ~USART_CR1_TXEIE; // tx int disable
    }
}

BOOL CPU_USART_TxBufferEmptyInterruptState( int ComPortNum )
{
    if (g_STM32_Uart[ComPortNum]->CR1 & USART_CR1_TXEIE) return TRUE;
    return FALSE;
}

void CPU_USART_RxBufferFullInterruptEnable( int ComPortNum, BOOL Enable )
{
    USART_TypeDef* uart = g_STM32_Uart[ComPortNum];
    if (Enable) {
        uart->CR1 |= USART_CR1_RXNEIE;  // rx int enable
    } else {
        uart->CR1 &= ~USART_CR1_RXNEIE; // rx int disable
    }
}

BOOL CPU_USART_RxBufferFullInterruptState( int ComPortNum )
{
    if (g_STM32_Uart[ComPortNum]->CR1 & USART_CR1_RXNEIE) return TRUE;

    return FALSE;
}

BOOL CPU_USART_TxHandshakeEnabledState( int ComPortNum )
{
    // The state of the CTS input only matters if Flow Control is enabled
    if (g_STM32_Uart[ComPortNum]->CR3 & USART_CR3_CTSE)
    {
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        return !CPU_GPIO_GetPinState((GPIO_PIN)g_STM32_UART_CtsPin[ComPortNum]); // CTS active
#else
        GPIO_PIN pin = 0x0B;
        if (ComPortNum == 1) pin = 0x00;
        else if (ComPortNum == 2) pin = 0x1D;
        return !CPU_GPIO_GetPinState(pin); // CTS active
#endif
    }
    return TRUE; // If this handshake input is not being used, it is assumed to be good
}

void CPU_USART_ProtectPins( int ComPortNum, BOOL On )  // idempotent
{
    if (On) {
        CPU_USART_RxBufferFullInterruptEnable(ComPortNum, FALSE);
        CPU_USART_TxBufferEmptyInterruptEnable(ComPortNum, FALSE);
    } else {
        CPU_USART_TxBufferEmptyInterruptEnable(ComPortNum, TRUE);
        CPU_USART_RxBufferFullInterruptEnable(ComPortNum, TRUE);
    }
}

UINT32 CPU_USART_PortsCount()
{
    return TOTAL_USART_PORT;
}

void CPU_USART_GetPins( int ComPortNum, GPIO_PIN& rxPin, GPIO_PIN& txPin, GPIO_PIN& ctsPin, GPIO_PIN& rtsPin )
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	rxPin = (GPIO_PIN)g_STM32_UART_RxPin[ComPortNum];
	txPin = (GPIO_PIN)g_STM32_UART_TxPin[ComPortNum];
	ctsPin = (GPIO_PIN)g_STM32_UART_CtsPin[ComPortNum];
	rtsPin = (GPIO_PIN)g_STM32_UART_RtsPin[ComPortNum];
#else
    if (ComPortNum == 0) {
        rxPin  = 0x0A; // A10
        txPin  = 0x09; // A9
        ctsPin = 0x0B; // A11
        rtsPin = 0x0C; // A12
    } else if (ComPortNum == 1) {
        rxPin  = 0x03; // A3
        txPin  = 0x02; // A2
        ctsPin = 0x00; // A0
        rtsPin = 0x01; // A1
    } else {
        rxPin  = 0x1B; // B11
        txPin  = 0x1A; // B10
        ctsPin = 0x1D; // B13
        rtsPin = 0x1E; // B14
    }
#endif
}

void CPU_USART_GetBaudrateBoundary( int ComPortNum, UINT32 & maxBaudrateHz, UINT32 & minBaudrateHz )
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	USART_TypeDef* uart = g_STM32_Uart[ComPortNum];
	UINT32 clk;

    if(uart == USART1 || uart == USART6) { // USART1/USART6 on APB2
		clk = SYSTEM_APB2_CLOCK_HZ;
    } else { // USART2/USART3/UART4/UART5 on APB1
		clk = SYSTEM_APB1_CLOCK_HZ;
	}
#else
    UINT32 clk = SYSTEM_APB2_CLOCK_HZ;
    if (ComPortNum) clk = SYSTEM_APB1_CLOCK_HZ;
#endif

    maxBaudrateHz = clk >> 4;
    minBaudrateHz = clk >> 16;
}

BOOL CPU_USART_SupportNonStandardBaudRate( int ComPortNum )
{
    return TRUE;
}

BOOL CPU_USART_IsBaudrateSupported( int ComPortNum, UINT32& BaudrateHz )
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	USART_TypeDef* uart = g_STM32_Uart[ComPortNum];
	UINT32 clk;

    if(uart == USART1 || uart == USART6) { // USART1/USART6 on APB2
		clk = SYSTEM_APB2_CLOCK_HZ;
    } else { // USART2/USART3/UART4/UART5 on APB1
		clk = SYSTEM_APB1_CLOCK_HZ;
	}

	UINT32 max = clk >> 4;
#else
    UINT32 max = SYSTEM_APB2_CLOCK_HZ >> 4;
    if (ComPortNum) max = SYSTEM_APB1_CLOCK_HZ >> 4;
#endif

    if (BaudrateHz <= max) return TRUE;
    BaudrateHz = max;
    return FALSE;
}


