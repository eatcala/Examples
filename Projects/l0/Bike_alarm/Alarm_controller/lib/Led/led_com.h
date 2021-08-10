/******************************************************************************
 * @file led_com
 * @brief communication driver
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#ifndef LED_COM_H
#define LED_COM_H

#include "stm32f0xx_hal.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_system.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LED_TIM2_GPIO_CLK() __HAL_RCC_GPIOA_CLK_ENABLE()
#define LED_TIM2_INSTANCE   TIM2
#define LED_TIM2_PORT       GPIOA
#define LED_TIM2_CHANNEL    TIM_CHANNEL_1
#define LED_TIM2_CH_PIN     GPIO_PIN_0
#define LED_AF_TIM2         GPIO_AF2_TIM2

#define LED_TIM3_GPIO_CLK() __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED_TIM3_INSTANCE   TIM3
#define LED_TIM3_PORT       GPIOB
#define LED_TIM3_CHANNEL1   TIM_CHANNEL_1
#define LED_TIM3_CH1_PIN    GPIO_PIN_4
#define LED_TIM3_CHANNEL2   TIM_CHANNEL_2
#define LED_TIM3_CH2_PIN    GPIO_PIN_5
#define LED_AF_TIM3         GPIO_AF1_TIM3

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Function
 ******************************************************************************/

void LedCom_Init();

#endif /* LED_COM_H */