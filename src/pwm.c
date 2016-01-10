/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "pwm.h"

struct BLDC_pwm bldc_pwm;

void pwm_init (void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* Time Base configuration */
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
  TIM_TimeBaseStructure.TIM_Period = (2048 - 1); // 64MHz clock (PCLK1), 64MHz/4096 = 15.625KHz (BUT PWM center alined mode needs twice the frequency)
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 Configuration in PWM mode */
  TIM_OCInitTypeDef TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0; // start with 0% duty cycle
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 165; // 3us dead time
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

  /* TIM1 counter enable */
  TIM_Cmd (TIM1, ENABLE);

  /* TIM1 Main Output Disable */
  TIM_CtrlPWMOutputs (TIM1, DISABLE);
}

void pwm_set_duty_cycle (unsigned int value)
{
  // "value" = 1000 ---> 100% duty cycle
  // "value" = 425 ---> % 42.5% duty cycle

  // limit the input max value
  if (value >= DUTY_CYCLE_MAX)
  {
    value = DUTY_CYCLE_MAX;
  }

  // 1000 ---> 100%;
  // 0.625 * value = duty cycle (1000 accounts for max of 50%)
  //bldc_pwm.duty_cycle_normal = DUTY_CYCLE_MEDIUM + (value * 0.625); // this goes from 50% up to max
  //bldc_pwm.duty_cycle_inverted = DUTY_CYCLE_MEDIUM - (value * 0.625); // this goes from 50% down to max

  //bldc_pwm.duty_cycle_normal = DUTY_CYCLE_MEDIUM + (DUTY_CYCLE_MEDIUM / 10) + 55; // this goes from 50% up to max
  //bldc_pwm.duty_cycle_inverted = DUTY_CYCLE_MEDIUM - (DUTY_CYCLE_MEDIUM / 10) + 55; // this goes from 50% down to max

  bldc_pwm.duty_cycle_normal = 1023 + 302 + 110; //1023 + 55; // this goes from 50% up to max
  bldc_pwm.duty_cycle_inverted = 1023 - 302 + 110; // this goes from 50% down to max

  // check for invalid values
  //if (bldc_pwm.duty_cycle_normal > DUTY_CYCLE_MAX_SAFE)
  //{
    //bldc_pwm.duty_cycle_normal = DUTY_CYCLE_MAX_SAFE;
  //}

  //if (bldc_pwm.duty_cycle_inverted < DUTY_CYCLE_MIN_SAFE)
  //{
    //bldc_pwm.duty_cycle_inverted = DUTY_CYCLE_MIN_SAFE;
  //}

  pwm_update_duty_cycle ();
}


void pwm_update_duty_cycle (void)
{
  if (bldc_pwm.phase_a == NORMAL)
  {
    TIM_SetCompare3(TIM1, bldc_pwm.duty_cycle_normal); // phase A
  }
  else if (bldc_pwm.phase_a == INVERTED)
  {
    TIM_SetCompare3(TIM1, bldc_pwm.duty_cycle_inverted);
  }
  else if (bldc_pwm.phase_a == OFF)
  {
    TIM_SetCompare3(TIM1, DUTY_CYCLE_MEDIUM);
  }


  if (bldc_pwm.phase_b == NORMAL)
  {
    TIM_SetCompare1(TIM1, bldc_pwm.duty_cycle_normal); // phase B
  }
  else if (bldc_pwm.phase_b == INVERTED)
  {
    TIM_SetCompare1(TIM1, bldc_pwm.duty_cycle_inverted);
  }
  else if (bldc_pwm.phase_b == OFF)
  {
    TIM_SetCompare1(TIM1, DUTY_CYCLE_MEDIUM);
  }


  if (bldc_pwm.phase_c == NORMAL)
  {
    TIM_SetCompare2(TIM1, bldc_pwm.duty_cycle_normal); // phase C
  }
  else if (bldc_pwm.phase_c == INVERTED)
  {
    TIM_SetCompare2(TIM1, bldc_pwm.duty_cycle_inverted);
  }
  else if (bldc_pwm.phase_c == OFF)
  {
    TIM_SetCompare2(TIM1, DUTY_CYCLE_MEDIUM);
  }
}
