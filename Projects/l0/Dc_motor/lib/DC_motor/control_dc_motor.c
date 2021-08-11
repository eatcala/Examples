/******************************************************************************
 * @file controle_dc_motor
 * @brief pwm generation for dc motor control
 * @author Luos
 * @version 0.0.0
 ******************************************************************************/
#include "dc_motor.h"

#include "control_dc_motor.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
typedef struct
{
    TIM_TypeDef * CLKWiseTimer;
    uint8_t CLKWiseChannel;
    TIM_TypeDef * CounterCLKWiseTimer;
    uint8_t CounterCLKWiseChannel;
} Motor_control_t;
/*******************************************************************************
 * Variables
 ******************************************************************************/
Motor_control_t MotorDCControl[MOTOR_NUMBER];
TIM_HandleTypeDef Timerhandle;
/*******************************************************************************
 * Function
 ******************************************************************************/

/******************************************************************************
 * @brief Set
 * @param None
 * @return None
 ******************************************************************************/
void Control_Init(void)
{
    ///////////////////////////////
    //GPIO PIPE Init
    ///////////////////////////////
    PWM_PIN_CLK();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    GPIO_InitStruct.Pin       = PWM1_PIN;
    GPIO_InitStruct.Alternate = PWM1_AF;
    HAL_GPIO_Init(PWM1_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = PWM2_PIN;
    GPIO_InitStruct.Alternate = PWM2_AF;
    HAL_GPIO_Init(PWM2_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = PWM3_PIN;
    GPIO_InitStruct.Alternate = PWM3_AF;
    HAL_GPIO_Init(PWM3_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = PWM4_PIN;
    GPIO_InitStruct.Alternate = PWM4_AF;
    HAL_GPIO_Init(PWM4_PORT, &GPIO_InitStruct);

    ///////////////////////////////
    //Timer PWM Init
    ///////////////////////////////
    //initialize clock
    PWM_TIMER_CLK();

    TIM_MasterConfigTypeDef MasterConfig = {0};
    TIM_OC_InitTypeDef TimerConfigOC     = {0};

    Timerhandle.Init.Prescaler         = 0;
    Timerhandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    Timerhandle.Init.Period            = PWM_PERIOD;
    Timerhandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    Timerhandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    MasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    MasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;

    TimerConfigOC.OCMode     = TIM_OCMODE_PWM1;
    TimerConfigOC.Pulse      = 0;
    TimerConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    TimerConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    Timerhandle.Instance = PWM1_TIMER;
    while (HAL_TIM_PWM_Init(&Timerhandle) != HAL_OK)
        ;
    while (HAL_TIMEx_MasterConfigSynchronization(&Timerhandle, &MasterConfig) != HAL_OK)
        ;
    while (HAL_TIM_PWM_ConfigChannel(&Timerhandle, &TimerConfigOC, PWM1_CHANNEL) != HAL_OK)
        ;
    HAL_TIM_PWM_Start(&Timerhandle, PWM1_CHANNEL);

    Timerhandle.Instance = PWM2_TIMER;
    while (HAL_TIM_PWM_Init(&Timerhandle) != HAL_OK)
        ;
    while (HAL_TIMEx_MasterConfigSynchronization(&Timerhandle, &MasterConfig) != HAL_OK)
        ;
    while (HAL_TIM_PWM_ConfigChannel(&Timerhandle, &TimerConfigOC, PWM2_CHANNEL) != HAL_OK)
        ;
    HAL_TIM_PWM_Start(&Timerhandle, PWM2_CHANNEL);

    Timerhandle.Instance = PWM3_TIMER;
    while (HAL_TIM_PWM_Init(&Timerhandle) != HAL_OK)
        ;
    while (HAL_TIMEx_MasterConfigSynchronization(&Timerhandle, &MasterConfig) != HAL_OK)
        ;
    while (HAL_TIM_PWM_ConfigChannel(&Timerhandle, &TimerConfigOC, PWM3_CHANNEL) != HAL_OK)
        ;
    HAL_TIM_PWM_Start(&Timerhandle, PWM3_CHANNEL);


    Timerhandle.Instance = PWM4_TIMER;
    while (HAL_TIM_PWM_Init(&Timerhandle) != HAL_OK)
        ;
    while (HAL_TIMEx_MasterConfigSynchronization(&Timerhandle, &MasterConfig) != HAL_OK)
        ;
    while (HAL_TIM_PWM_ConfigChannel(&Timerhandle, &TimerConfigOC, PWM4_CHANNEL) != HAL_OK)
        ;
    HAL_TIM_PWM_Start(&Timerhandle, PWM4_CHANNEL);

    //assign to motor pwm 
    MotorDCControl[MOTOR_DC_1].CLKWiseTimer   = PWM1_TIMER;
    MotorDCControl[MOTOR_DC_1].CLKWiseChannel = (uint8_t)PWM1_CHANNEL;
    MotorDCControl[MOTOR_DC_1].CounterCLKWiseTimer   = PWM2_TIMER;
    MotorDCControl[MOTOR_DC_1].CounterCLKWiseChannel = (uint8_t)PWM2_CHANNEL;
    
    MotorDCControl[MOTOR_DC_2].CLKWiseTimer   = PWM3_TIMER;
    MotorDCControl[MOTOR_DC_2].CLKWiseChannel = (uint8_t)PWM3_CHANNEL;
    MotorDCControl[MOTOR_DC_2].CounterCLKWiseTimer   = PWM4_TIMER;
    MotorDCControl[MOTOR_DC_2].CounterCLKWiseChannel = (uint8_t)PWM4_CHANNEL;
}
/******************************************************************************
 * @brief Set Power to DC motor
 * @param None
 * @return None
 ******************************************************************************/
void Control_SetPower(uint8_t Motor, ratio_t power)
{
    uint16_t pulse;
    if (power > 0.0)
    {
        // limit power value
        if (power > 100.0)
        {
            power = 100.0;
        }
        // transform power ratio to timer value
        pulse = (uint16_t)(power * 50.0);
        Timerhandle.Instance = MotorDCControl[Motor].CLKWiseTimer;
        __HAL_TIM_SET_COMPARE(&Timerhandle, MotorDCControl[Motor].CLKWiseChannel, pulse);
        Timerhandle.Instance = MotorDCControl[Motor].CounterCLKWiseTimer;
        __HAL_TIM_SET_COMPARE(&Timerhandle, MotorDCControl[Motor].CounterCLKWiseChannel, 0);
    }
    else
    {
        // limit power value
        if (power < -100.0)
        {
            power = -100.0;
        }
        // transform power ratio to timer value
        pulse = (uint16_t)(-power * 50.0);
        Timerhandle.Instance = MotorDCControl[Motor].CounterCLKWiseTimer;
        __HAL_TIM_SET_COMPARE(&Timerhandle, MotorDCControl[Motor].CounterCLKWiseChannel, pulse);
        Timerhandle.Instance = MotorDCControl[Motor].CLKWiseTimer;
        __HAL_TIM_SET_COMPARE(&Timerhandle, MotorDCControl[Motor].CLKWiseChannel, 0);
    }
}
