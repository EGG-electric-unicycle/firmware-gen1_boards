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

  /* PWM Output Enable */
  TIM_CtrlPWMOutputs (TIM1, ENABLE);
}

void motor_coast (void)
{
  /* PWM Output Disable */
  TIM_CtrlPWMOutputs (TIM1, DISABLE);
}

void motor_set_duty_cycle (unsigned int value)
{
  update_duty_cycle (value);
}
