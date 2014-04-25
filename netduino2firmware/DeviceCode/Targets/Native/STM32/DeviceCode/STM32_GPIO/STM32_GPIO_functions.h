#ifndef _STM32__GPIO_FUNCTIONS_H
#define _STM32__GPIO_FUNCTIONS_H

#if defined(PLATFORM_ARM_STM32F2_ANY) || defined(PLATFORM_ARM_STM32F4_ANY)

//#define GPIO_AF_USART1        ((uint8_t)0x07)  /* USART1 Alternate Function mapping */
//#define GPIO_AF_USART2        ((uint8_t)0x07)  /* USART2 Alternate Function mapping */
//#define GPIO_AF_USART3        ((uint8_t)0x07)  /* USART3 Alternate Function mapping */
//#define GPIO_AF_I2S3ext       ((uint8_t)0x07)  /* I2S3ext Alternate Function mapping */

/**
  * @brief   AF 8 selection
  */
//#define GPIO_AF_UART4         ((uint8_t)0x08)  /* UART4 Alternate Function mapping */
//#define GPIO_AF_UART5         ((uint8_t)0x08)  /* UART5 Alternate Function mapping */
//#define GPIO_AF_USART6        ((uint8_t)0x08)  /* USART6 Alternate Function mapping */

void STM32_GPIO_AFConfig(UINT32 pin, UINT8 GPIO_AF);
#endif

#endif
