/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x.h"
#include "hall_sensor.h"
#include "bldc.h"
#include "pwm.h"
#include "main.h"

unsigned int motor_get_speed (void)
{
  unsigned int motor_speed;
  motor_speed = 1.0 / ((get_hall_sensors_us () * 6.0) / 1000000.0);
  return motor_speed;
}

void motor_set_speed (unsigned int speed)
{
  (void) speed;
}

void motor_start (void)
{
  commutate (); // starts the commutation
  start_adc_max_current_management ();
  TIM_CtrlPWMOutputs (TIM1, ENABLE); // PWM Output Enable
  machine_state = RUNNING;
}

void motor_coast (void)
{
  stop_adc_max_current_management ();
  TIM_CtrlPWMOutputs (TIM1, DISABLE); // PWM Output Disable
  machine_state = COAST;
}

void motor_set_duty_cycle (unsigned int value)
{
  update_duty_cycle (value);
}

unsigned int motor_get_current (void)
{
  unsigned int adc_current;
  unsigned int current;


  adc_current = adc_get_current_value ();

  /*
   * 1A = 0.0385V
   * (350W nominal motor power) 5.8A = 0.2233V
   * (350*1.5W peak motor power ??) = 0.335V
   *
   * 12 bits ADC; 4096 steps
   * 3.3V / 4096 = 0.0008
   *
   */
   current = adc_current * 0.0008;

   return current;
}

void TIM1_BRK_IRQHandler (void)
{
  TIM_CtrlPWMOutputs (TIM1, DISABLE); //disable PWM signals
  TIM_ITConfig (TIM1, TIM_IT_Break, ENABLE); //disable this interrupt
  machine_state = OVER_CURRENT;

  // clear interrupt flag
  TIM_ClearITPendingBit (TIM1, TIM_IT_Break);
}

void brake_init (void)
{
  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Disable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Disable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
  TIM_BDTRInitStructure.TIM_DeadTime = 0;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable Break interrupt */
  TIM_ITConfig (TIM1, TIM_IT_Break, ENABLE);
}

