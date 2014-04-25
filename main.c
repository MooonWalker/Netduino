

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"




volatile uint32_t msTicks;


/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void)
{
  msTicks++;
}

void Delay (uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

void init_GPIO(void)
	{
		/* This TypeDef is a structure defined in the
	 * ST's library and it contains all the properties
	 * the corresponding peripheral has, such as output mode,
	 * pullup / pulldown resistors etc.
	 *
	 * These structures are defined for every peripheral so
	 * every peripheral has it's own TypeDef. The good news is
	 * they always work the same so once you've got a hang
	 * of it you can initialize any peripheral.
	 *
	 * The properties of the periperals can be found in the corresponding
	 * header file e.g. stm32f4xx_gpio.h and the source file stm32f4xx_gpio.c
	 */
		GPIO_InitTypeDef GPIO_InitStruct;

		/* This enables the peripheral clock to
	 * the GPIOA IO module
	 */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // we want to configure PA10
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_OType=GPIO_OType_PP;
		GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOA, &GPIO_InitStruct);
	}



int main(void)
{
	//volatile unsigned int i=0;

	SystemInit();
	SystemCoreClockUpdate();
	if (SysTick_Config(SystemCoreClock / 1000))
	{ /* SysTick 1 msec interrupts  */
	   while (1);                                  /* Capture error              */
	}

	init_GPIO();

	//RCC ->AHB1ENR |= ( 1<<0 ); // enable clk for PORT A
	//GPIOA ->MODER |= (1<<20); // enable Port A pin10 as output

	//GPIOA ->ODR |= (0<<10); // GPIOA pin10 OFF
	
    while(1)
    {
    	Delay(900);
    	//for (i = 0; i < 900000; ++i);
    	GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_SET);
        //GPIOA ->ODR |= (1<<10); // GPIOA pin10 ON

    	Delay(200);
    	//for (i = 0; i < 200000; ++i);
    	GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_RESET);
        //GPIOA ->ODR &= ~(1<<10); // GPIOA pin10 OFF toggle
    }
}
