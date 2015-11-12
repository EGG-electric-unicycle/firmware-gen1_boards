
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

/* Connections:
 *
 * PIN				| IN/OUT|	Function
 * -------------------------------------
 *
 * PA8  (TIM1_CH1)  | out	| Bridge_A-High
 * PA9  (TIM1_CH2)  | out	| Bridge_B-High
 * PA10 (TIM1_CH3)  | out	| Bridge_C-High
 * PB13 (TIM1_CH1N) | out	| Bridge_A-Low
 * PB14 (TIM1_CH2N) | out	| Bridge_B-Low
 * PB15 (TIM1_CH3N) | out	| Bridge_C-Low
 *
 * PA2  (TIM2_CH3) 	| in	| Hall_sensor_A
 * PA1  (TIM2_CH2) 	| in	| Hall_sensor_B
 * PA0  (TIM2_CH1) 	| in	| Hall_sensor_C

 * PA6  (ADC12_IN6) | in	| BMF_signal-Yellow_A
 * PB0  (ADC12_IN8) | in	| BMF_signal-Green_B
 * PB1  (ADC12_IN9) | in	| BMF_signal-Blue_C
 *
 * PA7  (ADC12_IN7)	| in	| Current_signal
 *
 * PB6  (I2C1_SCL)	| in/out| IMU_MPU6050-SCL
 * PB7  (I2C1_SDA)	| in/out| IMU_MPU6050-SDA
 *
 * PA15 			| out	| LED_1-battery_indicator
 * PB4	 			| out	| LED_2-battery_indicator
 * PB5	 			| out	| LED_3-battery_indicator
 * PB8	 			| out	| LED_4-battery_indicator
 * PB9	 			| out	| LED-power_switcher
 *
 * PB3	 			| out	| Buzzer(??)
 * PA4	 			| out	| PS_signal(calibrate_wheel??)
 *
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
