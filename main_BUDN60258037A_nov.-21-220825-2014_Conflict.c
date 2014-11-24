#include "stm32f4xx_conf.h"
#include "ARMduino.h"



void SysTick_Handler(void)
{
  msTicks++;
}


void init_GPIO(void)
	{
		GPIO_InitTypeDef GPIO_InitStruct;

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
	SystemInit();  //ARM init

	//arduino setup()...
	SystemCoreClockUpdate();
	if (SysTick_Config(SystemCoreClock / 1000))
	{ /* SysTick 1 msec interrupts  */
	   while (1);                                  /* Capture error              */
	}

	init_GPIO();
	GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_SET);
//arduino loop()...
    while(1)
    {
    	Delay(1500);

    	GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_SET);


    	Delay(100);

    	GPIO_WriteBit(GPIOA, GPIO_Pin_10, Bit_RESET);

    }
}
