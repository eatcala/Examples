/******************************************************************************
 * @file dc_motor
 * @brief driver example a simple dc_motor
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#ifndef CONTROL_DC_MOTOR_H
#define CONTROL_DC_MOTOR_H

#include "stm32f0xx_hal.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
enum
{
    MOTOR_DC_1,
    MOTOR_DC_2,
    MOTOR_NUMBER,
};

#define PWM_PIN_CLK()                 \
    do                                \
    {                                 \
        __HAL_RCC_GPIOA_CLK_ENABLE(); \
        __HAL_RCC_GPIOB_CLK_ENABLE(); \
    } while (0U)

#define PWM_TIMER_CLK()               \
    do                                \
    {                                 \
        __HAL_RCC_TIM2_CLK_DISABLE(); \
        __HAL_RCC_TIM3_CLK_DISABLE(); \
    } while (0U)

#define PWM_PERIOD 5000-1

//MOTOR_DC_1
#define PWM1_PIN  GPIO_PIN_0
#define PWM1_PORT GPIOA
#define PWM1_AF   GPIO_AF2_TIM2

#define PWM2_PIN  GPIO_PIN_3
#define PWM2_PORT GPIOB
#define PWM2_AF   GPIO_AF2_TIM2

#define PWM1_TIMER   TIM2
#define PWM1_CHANNEL TIM_CHANNEL_1

#define PWM2_TIMER   TIM2
#define PWM2_CHANNEL TIM_CHANNEL_2

//MOTOR_DC_2
#define PWM3_PIN  GPIO_PIN_4
#define PWM3_PORT GPIOB
#define PWM3_AF   GPIO_AF1_TIM3

#define PWM4_PIN  GPIO_PIN_5
#define PWM4_PORT GPIOB
#define PWM4_AF   GPIO_AF1_TIM3

#define PWM3_TIMER   TIM3
#define PWM3_CHANNEL TIM_CHANNEL_1

#define PWM4_TIMER   TIM3
#define PWM4_CHANNEL TIM_CHANNEL_2

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Function
 ******************************************************************************/
void Control_Init(void);
void Control_SetPower(uint8_t Motor, ratio_t power);

#endif /* CONTROL_DC_MOTOR_H */
