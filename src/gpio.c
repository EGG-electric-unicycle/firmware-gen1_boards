
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/* Connections:
 *
 * PA2  (ADC12_IN7)     -- Current_signal
 * PA8  (TIM1_CH1)      -- Bridge_A-High
 * PA9  (TIM1_CH2)      -- Bridge_B-High
 * PA10 (TIM1_CH3)      -- Bridge_C-High
 * PB13 (TIM1_CH1N)     -- Bridge_A-Low
 * PB14 (TIM1_CH2N)     -- Bridge_B-Low
 * PB15 (TIM1_CH3N)     -- Bridge_C-Low
 * PA2  (TIM3_CH1)      -- Hall_sensor_A
 * PA1  (TIM3_CH2)      -- Hall_sensor_B
 * PA0  (TIM3_CH3)      -- Hall_sensor_C
 */


void gpio_init (void)
{
#if 0
  /* Enable GPIOB clock. */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO  |
                          RCC_APB2Periph_GPIOA |
                          RCC_APB2Periph_GPIOB |
                          RCC_APB2Periph_TIM1  |
                          RCC_APB2Periph_ADC1
                          , ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_DAC   |
                          RCC_APB1Periph_TIM3  |
                          RCC_APB1Periph_USART3
                          , ENABLE);

  /* Once the DAC channel is enabled, the corresponding GPIO pin is automatically
     connected to the DAC converter. In order to avoid parasitic consumption,
     the GPIO pin should be configured in analog */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOA Configuration: Channel 1, 2 and 3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  PWM 6
  /* Turn off port bits */
  GPIO_ResetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
#endif
}
