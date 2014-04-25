#include "stm32f4xx.h"



int main(void)
{
	volatile unsigned int i=0;
	
	RCC ->AHB1ENR |= ( 1<<0 ); // enable clk for PORT A
	GPIOA ->MODER |= (1<<20); // enable Port A pin10 as output

	GPIOA ->ODR |= (0<<10); // GPIOA pin10 OFF
	
    while(1)
    {
    	for (i = 0; i < 900000; ++i);		
    	GPIOA ->ODR |= (1<<10); // GPIOA pin10 ON
    	for (i = 0; i < 200000; ++i);
    	GPIOA ->ODR &= ~(1<<10); // GPIOA pin10 OFF toggle
    }
}

