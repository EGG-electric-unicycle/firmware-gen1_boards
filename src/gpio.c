/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

/* Connections:
 *
 * Motor PHASE_A: green wire
 * Motor PHASE_B: yellow wire
 * Motor PHASE_B: blue wire
 *
 *
 * PIN				| IN/OUT|	Function
 * ----------------------------------------------------------
 *
 * PA8  (TIM1_CH1)  		| out	| Bridge_A-High (green wire)
 * PA9  (TIM1_CH2)  		| out	| Bridge_B-High
 * PA10 (TIM1_CH3)  		| out	| Bridge_C-High
 * PB13 (TIM1_CH1N) 		| out	| Bridge_A-Low
 * PB14 (TIM1_CH2N) 		| out	| Bridge_B-Low
 * PB15 (TIM1_CH3N) 		| out	| Bridge_C-Low
 *
 * PA2  (TIM2_CH3) 		| in	| Hall_sensor_A
 * PA1  (TIM2_CH2) 		| in	| Hall_sensor_B
 * PA0  (TIM2_CH1) 		| in	| Hall_sensor_C

 * PA6  (ADC12_IN6) 		| in	| BMF_signal-Yellow_A
 * PB0  (ADC12_IN8) 		| in	| BMF_signal-Green_B
 * PB1  (ADC12_IN9) 		| in	| BMF_signal-Blue_C
 *
 * PA7  (ADC12_IN7)		| in	| Current_signal
 *
 * PB6  (I2C1_SCL)		| in/out| IMU_MPU6050-SCL
 * PB7  (I2C1_SDA)		| in/out| IMU_MPU6050-SDA
 *
 * PA15 			| out	| LED_1-battery_indicator (active low)
 * PB4	 			| out	| LED_2-battery_indicator (active low)
 * PB5	 			| out	| LED_3-battery_indicator (active low)
 * PB8	 			| out	| LED_4-battery_indicator (active low)
 * PB9	 			| out	| LED-power_switcher (active low)
 *
 * PB3	 			| out	| Buzzer(??)
 * PA4	 			| out	| PS_signal(calibrate_wheel??)
 *
 */

GPIO_InitTypeDef GPIO_InitStructure;

void gpio_init (void)
{
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

  /* Enable clocks */
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO  |
                          RCC_APB2Periph_GPIOA |
                          RCC_APB2Periph_GPIOB
                          , ENABLE);

  /* Configure pins for the battery indicator LEDs */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // disable the LEDs
  GPIO_SetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9);
  GPIO_SetBits(GPIOA, GPIO_Pin_15);
  /***/

  /* Configure pins for the hall sensors */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
