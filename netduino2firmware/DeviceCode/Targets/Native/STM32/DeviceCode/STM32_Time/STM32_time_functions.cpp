////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for STM32: Copyright (c) Oberon microsystems, Inc.
//
//  *** System Timer Driver ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cores\arm\include\cpu.h>
#if defined(PLATFORM_ARM_STM32F4_ANY)
#include "..\stm32f4xx.h"
#elif defined(PLATFORM_ARM_STM32F2_ANY)
#include "..\stm32f2xx.h"
#else
#include "..\stm32f10x.h"
#endif


static UINT64 g_nextEvent;   // tick time of next event to be scheduled
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
static UINT64 g_overflowCounter;
#endif

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	#if STM32_32BIT_TIMER == 2
		#define TIM_32BIT TIM2
		#define TIM_32BIT_IRQn TIM2_IRQn
		#define RCC_APB1ENR_TIM_32BIT_EN RCC_APB1ENR_TIM2EN
	#elif STM32_32BIT_TIMER == 5
		#define TIM_32BIT TIM5
		#define TIM_32BIT_IRQn TIM5_IRQn
		#define RCC_APB1ENR_TIM_32BIT_EN RCC_APB1ENR_TIM5EN
	#endif
#endif

UINT32 CPU_SystemClock()
{
    return SYSTEM_CLOCK_HZ;
}

UINT32 CPU_TicksPerSecond()
{
    return SLOW_CLOCKS_PER_SECOND;
}

//--//

UINT64 CPU_TicksToTime( UINT64 Ticks )
{
    Ticks *= (TEN_MHZ               /SLOW_CLOCKS_TEN_MHZ_GCD);
    Ticks /= (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_TEN_MHZ_GCD);

    return Ticks;
}

UINT64 CPU_TicksToTime( UINT32 Ticks32 )
{
    UINT64 Ticks;

    Ticks  = (UINT64)Ticks32 * (TEN_MHZ               /SLOW_CLOCKS_TEN_MHZ_GCD);
    Ticks /=                   (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_TEN_MHZ_GCD);

    return Ticks;
}

//--//

UINT64 CPU_MillisecondsToTicks( UINT64 Ticks )
{
    Ticks *= (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_MILLISECOND_GCD);
    Ticks /= (1000                  /SLOW_CLOCKS_MILLISECOND_GCD);

    return Ticks;
}

UINT64 CPU_MillisecondsToTicks( UINT32 Ticks32 )
{
    UINT64 Ticks;

    Ticks  = (UINT64)Ticks32 * (SLOW_CLOCKS_PER_SECOND/SLOW_CLOCKS_MILLISECOND_GCD);
    Ticks /=                   (1000                  /SLOW_CLOCKS_MILLISECOND_GCD);

    return Ticks;
}

//--//

#pragma arm section code = "SectionForFlashOperations"

UINT64 __section(SectionForFlashOperations) CPU_MicrosecondsToTicks( UINT64 uSec )
{
#if ONE_MHZ <= SLOW_CLOCKS_PER_SECOND
    return uSec * (SLOW_CLOCKS_PER_SECOND / ONE_MHZ);
#else
    return uSec / (ONE_MHZ / SLOW_CLOCKS_PER_SECOND);
#endif
}

UINT32 __section(SectionForFlashOperations) CPU_MicrosecondsToTicks( UINT32 uSec )
{
#if ONE_MHZ <= SLOW_CLOCKS_PER_SECOND
    return uSec * (SLOW_CLOCKS_PER_SECOND / ONE_MHZ);
#else
    return uSec / (ONE_MHZ / SLOW_CLOCKS_PER_SECOND);
#endif
}

#pragma arm section code

//--//

UINT32 CPU_MicrosecondsToSystemClocks( UINT32 uSec )
{
    uSec *= (SYSTEM_CLOCK_HZ/CLOCK_COMMON_FACTOR);
    uSec /= (ONE_MHZ        /CLOCK_COMMON_FACTOR);

    return uSec;
}

int CPU_MicrosecondsToSystemClocks( int uSec )
{
    uSec *= (SYSTEM_CLOCK_HZ/CLOCK_COMMON_FACTOR);
    uSec /= (ONE_MHZ        /CLOCK_COMMON_FACTOR);

    return uSec;
}

//--//

int CPU_SystemClocksToMicroseconds( int Ticks )
{
    Ticks *= (ONE_MHZ        /CLOCK_COMMON_FACTOR);
    Ticks /= (SYSTEM_CLOCK_HZ/CLOCK_COMMON_FACTOR);

    return Ticks;
}

//--//


void Timer_Interrupt (void* param) // timer2 compare event
{
    INTERRUPT_START

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    TIM_32BIT->SR &= ~TIM_SR_CC1IF; // reset interrupt flag
#else
    TIM2->SR = ~TIM_SR_CC1IF; // reset interrupt flag
#endif

    if (HAL_Time_CurrentTicks() >= g_nextEvent) { // handle event
       HAL_COMPLETION::DequeueAndExec(); // this also schedules the next one, if there is one
    }

    INTERRUPT_END
}


#pragma arm section code = "SectionForFlashOperations"

UINT64 __section(SectionForFlashOperations) HAL_Time_CurrentTicks()
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
	//g_overflowCounter &= (0xFFFFFFFFFFFF0000ull); // 16-bit
	//g_overflowCounter &= (0xFFFFFFFF00000000ull); // 32-bit

	if(TIM_32BIT->SR & TIM_SR_UIF)
	{
		TIM_32BIT->SR = ~TIM_SR_UIF;
//    	g_overflowCounter += (0x1ull << 16); // 16-bit
    	g_overflowCounter += (0x1ull << 32); // 32-bit
	}

    return g_overflowCounter | TIM_32BIT->CNT;
#else
    UINT32 t2, t3, t4; // cascaded timers
    do {
        t3 = TIM3->CNT;
        t4 = TIM4->CNT;
        t2 = TIM2->CNT;
    } while (t3 != TIM3->CNT); // asure consistent values
    return t2 | t3 << 16 | (UINT64)t4 << 32;
#endif
}

#pragma arm section code

void HAL_Time_SetCompare( UINT64 CompareValue )
{
    GLOBAL_LOCK(irq);
    g_nextEvent = CompareValue;
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
//    TIM_32BIT->CCR1 = (UINT16)CompareValue; // compare to low bits of CompareValue
    TIM_32BIT->CCR1 = (UINT32)CompareValue; // compare to low bits of CompareValue
#else
    TIM2->CCR1 = (UINT16)CompareValue; // compare to low bits
#endif

    if (HAL_Time_CurrentTicks() >= CompareValue) { // missed event
        // trigger immediate interrupt
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
        TIM_32BIT->EGR = TIM_EGR_CC1G; // trigger compare1 event
#else
        TIM2->EGR = TIM_EGR_CC1G; // trigger compare1 event
#endif
    }
}

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
// NOTE: both 32-bit clocks (TIM2 and TIM5) are based on APB1
#if SYSTEM_APB1_CLOCK_HZ == SYSTEM_CYCLE_CLOCK_HZ
#define TIM_32BIT_CLK_HZ SYSTEM_APB1_CLOCK_HZ
#else
#define TIM_32BIT_CLK_HZ (SYSTEM_APB1_CLOCK_HZ * 2)
#endif
#else
#if SYSTEM_APB1_CLOCK_HZ == SYSTEM_CYCLE_CLOCK_HZ
#define TIM2CLK_HZ SYSTEM_APB1_CLOCK_HZ
#else
#define TIM2CLK_HZ (SYSTEM_APB1_CLOCK_HZ * 2)
#endif
#endif

BOOL HAL_Time_Initialize()
{
    g_nextEvent = 0xFFFFFFFFFFFF; // never
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    g_overflowCounter = 0;

    // enable single timer
    RCC->APB1ENR |= RCC_APB1ENR_TIM_32BIT_EN;

    // 32-bit timer
    TIM_32BIT->CR1 = TIM_CR1_URS;
//    TIM_32BIT->CR2 = TIM_CR2_MMS_1; // master mode selection = update event <-- used if chaining timers
    TIM_32BIT->CR2 = 0; 
    TIM_32BIT->SMCR = 0; // TS = 000, SMS = 000, internal clock
    TIM_32BIT->DIER = TIM_DIER_CC1IE; // enable compare1 interrupt
    TIM_32BIT->CCMR1 = 0; // compare, no outputs
    TIM_32BIT->CCMR2 = 0; // compare, no outputs
    TIM_32BIT->PSC = (TIM_32BIT_CLK_HZ / SLOW_CLOCKS_PER_SECOND) - 1; // prescaler to 1MHz
    TIM_32BIT->CCR1 = 0;
//    TIM_32BIT->ARR = 0xFFFF; // 16-bit
    TIM_32BIT->ARR = 0xFFFFFFFF; // 32-bit
    TIM_32BIT->EGR = TIM_EGR_UG; // enforce first update

    TIM_32BIT->CR1 |= TIM_CR1_CEN;

    return CPU_INTC_ActivateInterrupt(TIM_32BIT_IRQn, Timer_Interrupt, 0);
#else
    g_nextEvent = 0xFFFFFFFFFFFF; // never
    
    // enable timer 2-4 clocks
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    
    // timer2
    TIM2->CR1 = TIM_CR1_URS;
    TIM2->CR2 = TIM_CR2_MMS_1; // master mode selection = update event
    TIM2->SMCR = 0; // TS = 000, SMS = 000, internal clock
    TIM2->DIER = TIM_DIER_CC1IE; // enable compare1 interrupt
    TIM2->CCMR1 = 0; // compare, no outputs
    TIM2->CCMR2 = 0; // compare, no outputs
    TIM2->PSC = (TIM2CLK_HZ / SLOW_CLOCKS_PER_SECOND) - 1; // prescaler to 1MHz
    TIM2->CCR1 = 0;
    TIM2->ARR = 0xFFFF;
    TIM2->EGR = TIM_EGR_UG; // enforce first update
    
    // timer3
    TIM3->CR1 = TIM_CR1_URS;
    TIM3->CR2 = TIM_CR2_MMS_1; // master mode selection = update event
    TIM3->SMCR = TIM_SMCR_TS_0 // TS = 001, clock = timer2 update
                 | TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2; // SMS = 111
    TIM3->DIER = 0; // no interrupt
    TIM3->PSC = 0;  // no prescaler
    TIM3->ARR = 0xFFFF;
    TIM3->EGR = TIM_EGR_UG; // enforce first update
    
    // timer4
    TIM4->CR1 = TIM_CR1_URS;
    TIM4->CR2 = 0;
    TIM4->SMCR = TIM_SMCR_TS_1 // TS = 010, clock = timer3 update
                 | TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2; // SMS = 111
    TIM4->DIER = 0; // no interrupt
    TIM4->PSC = 0;  // no prescaler
    TIM4->ARR = 0xFFFF;
    TIM4->EGR = TIM_EGR_UG; // enforce first update
    
    TIM4->CR1 |= TIM_CR1_CEN; // enable timers
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM2->CR1 |= TIM_CR1_CEN;
    
    return CPU_INTC_ActivateInterrupt(TIM2_IRQn, Timer_Interrupt, 0);
#endif
}

BOOL HAL_Time_Uninitialize()
{
#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)
    CPU_INTC_DeactivateInterrupt(TIM_32BIT_IRQn);

    TIM_32BIT->CR1 &= ~TIM_CR1_CEN;

    // disable timer 2-4 clocks
    RCC->APB1ENR &= ~(RCC_APB1ENR_TIM_32BIT_EN);
#else
    CPU_INTC_DeactivateInterrupt(TIM2_IRQn);
    
    TIM4->CR1 &= ~TIM_CR1_CEN; // disable timers
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    
    // disable timer 2-4 clocks
    RCC->APB1ENR &= ~(RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN);
#endif

    return TRUE;
}


#pragma arm section code = "SectionForFlashOperations"

//
// To calibrate this constant, uncomment #define CALIBRATE_SLEEP_USEC in TinyHAL.c
//
#define STM32_SLEEP_USEC_FIXED_OVERHEAD_CLOCKS 3

void __section(SectionForFlashOperations) HAL_Time_Sleep_MicroSeconds( UINT32 uSec )
{
    GLOBAL_LOCK(irq);

    UINT32 current   = HAL_Time_CurrentTicks();
    UINT32 maxDiff = CPU_MicrosecondsToTicks( uSec );

    if(maxDiff <= STM32_SLEEP_USEC_FIXED_OVERHEAD_CLOCKS) maxDiff  = 0;
    else maxDiff -= STM32_SLEEP_USEC_FIXED_OVERHEAD_CLOCKS;

    while(((INT32)(HAL_Time_CurrentTicks() - current)) <= maxDiff);
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled( UINT32 uSec )
{
    // iterations must be signed so that negative iterations will result in the minimum delay

    uSec *= (SYSTEM_CYCLE_CLOCK_HZ / CLOCK_COMMON_FACTOR);
    uSec /= (ONE_MHZ               / CLOCK_COMMON_FACTOR);

    // iterations is equal to the number of CPU instruction cycles in the required time minus
    // overhead cycles required to call this subroutine.
    int iterations = (int)uSec - 5;      // Subtract off call & calculation overhead

    CYCLE_DELAY_LOOP(iterations);
}

#pragma arm section code

INT64 HAL_Time_TicksToTime( UINT64 Ticks )
{
    return CPU_TicksToTime( Ticks );
}

INT64 HAL_Time_CurrentTime()
{
    return CPU_TicksToTime( HAL_Time_CurrentTicks() );
}

void HAL_Time_GetDriftParameters  ( INT32* a, INT32* b, INT64* c )
{
    *a = 1;
    *b = 1;
    *c = 0;
}
