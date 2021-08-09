#include "led_com.h"

void LedCom_Init(void)
{

    LED_RX_CLK();
    LED_TX_CLK();

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* Peripheral clock enable */
    //LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);
    /**USART1 GPIO Configuration
  PA9   ------> USART1_TX
  PA10   ------> USART1_RX
  */
    GPIO_InitStruct.Pin        = LED_RX_PIN;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate  = LED_RX_AF;
    HAL_GPIO_Init(LED_RX_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin        = LED_TX_PIN;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate  = LED_TX_AF;
    HAL_GPIO_Init(LED_TX_PORT, &GPIO_InitStruct);

    LED_COM_CLOCK_ENABLE();

    LL_USART_InitTypeDef USART_InitStruct = {0};

    /* USART1 interrupt Init */
    NVIC_SetPriority(LED_COM_IRQ, 0);
    NVIC_EnableIRQ(LED_COM_IRQ);

    USART_InitStruct.BaudRate            = 57600;
    USART_InitStruct.DataWidth           = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits            = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity              = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection   = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling        = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(LED_COM, &USART_InitStruct);
    LL_USART_DisableIT_CTS(LED_COM);
    LL_USART_DisableOverrunDetect(LED_COM);
    LL_USART_ConfigAsyncMode(LED_COM);
    LL_USART_Enable(LED_COM);
}
