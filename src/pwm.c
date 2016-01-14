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
#include "bldc.h"

int pwm_duty_cycle;
int pwm_duty_cycle_target;

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

// Function to update the duty cycle PWM values on the PWM controller
void pwm_update_duty_cycle (void)
{
  #define DUTY_CYCLE_MAGIC_NUMBER		100 // this value was found experimentally and need to be added to duty cyce value to have correct output value

  // 2039 max duty cycle value
  #define DUTY_CYCLE_NORMAL_MAX		2047
  #define DUTY_CYCLE_NORMAL_MIN		(2047/2)
  #define DUTY_CYCLE_OFF		(2047/2) + DUTY_CYCLE_MAGIC_NUMBER
  #define DUTY_CYCLE_INVERTED_MAX	((2047/2) - 1)
  #define DUTY_CYCLE_INVERTED_MIN 0

//  #define DUTY_CYCLE_MAX_SAFE		(DUTY_CYCLE_MAX - (DUTY_CYCLE_MAX/20)) // 95% for safe margin
//  #define DUTY_CYCLE_MIN_SAFE		(DUTY_CYCLE_MAX/20 + DUTY_CYCLE_MAGIC_NUMBER) // 5% for safe margin

  unsigned int duty_cycle_normal;
  unsigned int duty_cycle_inverted;
  int temp;

  // Calc the correct value of duty cycle and inverted duty cycle
  temp = (pwm_duty_cycle + 999) * 1.024; // scale to correct value
  duty_cycle_normal = (unsigned int) (temp + DUTY_CYCLE_MAGIC_NUMBER);
  duty_cycle_inverted = 2047 - duty_cycle_normal;

  // Apply the duty cycle values
  if (bldc_phase_state.a == NORMAL)
  {
    TIM_SetCompare3(TIM1, duty_cycle_normal); // phase A
  }
  else if (bldc_phase_state.a == INVERTED)
  {
    TIM_SetCompare3(TIM1, duty_cycle_inverted);
  }
  else if (bldc_phase_state.a == OFF)
  {
    TIM_SetCompare3(TIM1, DUTY_CYCLE_OFF);
  }


  if (bldc_phase_state.b == NORMAL)
  {
    TIM_SetCompare1(TIM1, duty_cycle_normal); // phase B
  }
  else if (bldc_phase_state.b == INVERTED)
  {
    TIM_SetCompare1(TIM1, duty_cycle_inverted);
  }
  else if (bldc_phase_state.b == OFF)
  {
    TIM_SetCompare1(TIM1, DUTY_CYCLE_OFF);
  }


  if (bldc_phase_state.c == NORMAL)
  {
    TIM_SetCompare2(TIM1, duty_cycle_normal); // phase C
  }
  else if (bldc_phase_state.c == INVERTED)
  {
    TIM_SetCompare2(TIM1, duty_cycle_inverted);
  }
  else if (bldc_phase_state.c == OFF)
  {
    TIM_SetCompare2(TIM1, DUTY_CYCLE_OFF);
  }
}

// Function to set duty cycle PWM value
void pwm_set_duty_cycle (int value)
{
#define DUTY_CYCLE_MAX_VALUE	1000 //
#define DUTY_CYCLE_MIN_VALUE	-999 //

  // limit the input values
  if (value >= DUTY_CYCLE_MAX_VALUE)
  {
    value = DUTY_CYCLE_MAX_VALUE;
  }
  else if (value <= DUTY_CYCLE_MIN_VALUE)
  {
    value = DUTY_CYCLE_MIN_VALUE;
  }

  pwm_duty_cycle_target = value;
}

// This function need to be called every 1ms
// manages the increase/decrease of PWM value at rate
void pwm_manage (void)
{
  if (pwm_duty_cycle == pwm_duty_cycle_target)
  {
    return; // nothing to do, return
  }
  else if (pwm_duty_cycle < pwm_duty_cycle_target)
  {
    pwm_duty_cycle += PWM_DUTY_CYCLE_STEP;
  }
  else if (pwm_duty_cycle > pwm_duty_cycle_target)
  {
    pwm_duty_cycle -= PWM_DUTY_CYCLE_STEP;
  }

  pwm_update_duty_cycle ();
}
