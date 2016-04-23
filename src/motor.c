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
#include "bldc.h"

void motor_start (void)
{
  TIM_CtrlPWMOutputs (TIM1, ENABLE); // enable now the signals to motor

  TIM_Cmd (TIM1, ENABLE); // start now the PWM / motor control

  machine_state = RUNNING;
}

void motor_coast (void)
{
  // TODO stop_adc_max_current_management ();
  TIM_CtrlPWMOutputs (TIM1, DISABLE); // PWM Output Disable
  machine_state = COAST;
}

void motor_set_duty_cycle (int value)
{
  if (value <= 0)
  {
    value *= -1; // invert the signal if duty_cycle is negative, ie: -999 * -1 = 999
    bldc_set_direction (LEFT);
  }
  else
  {
    bldc_set_direction (RIGHT);
  }

  pwm_set_duty_cycle ((unsigned int) value);
}

void TIM1_BRK_IRQHandler (void)
{
  TIM_CtrlPWMOutputs (TIM1, DISABLE); //disable PWM signals
  TIM_ITConfig (TIM1, TIM_IT_Break, ENABLE); //disable this interrupt
  machine_state = OVER_CURRENT;

  // clear interrupt flag
  TIM_ClearITPendingBit (TIM1, TIM_IT_Break);

  while (1) ;
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

void motor_set_direction (unsigned int direction)
{
  bldc_set_direction (direction);
}

unsigned int motor_get_direction (void)
{
  return bldc_get_direction ();
}

