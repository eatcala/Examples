/******************************************************************************
 * @file led_com
 * @brief communication driver
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/

#ifndef LED_COM_H
#define LED_COM_H

#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_usart.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
//#include "stm32f0xx_ll_dma.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_hal.h"

#define LED_RX_CLK() __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED_RX_PIN   GPIO_PIN_9
#define LED_RX_PORT  GPIOA
#define LED_RX_AF    LL_GPIO_AF_1

#define LED_TX_CLK() __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED_TX_PIN   GPIO_PIN_10
#define LED_TX_PORT  GPIOA
#define LED_TX_AF    LL_GPIO_AF_1

#define LED_COM_CLOCK_ENABLE() __HAL_RCC_USART1_CLK_ENABLE()
#define LED_COM                USART1
#define LED_COM_IRQ            USART1_IRQn
#define LED_COM_IRQHANDLER()   USART1_IRQHandler()

#endif /* LED_COM_H */