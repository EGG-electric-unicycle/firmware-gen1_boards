/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#ifndef _PWM_H_
#define _PWM_H_

#define DUTY_CYCLE_MAX			(2048 / 2) // 100% duty cycle
#define DUTY_CYCLE_MEDIUM		((2048 / 2) + 110) // 50% duty cycle
#define DUTY_CYCLE_MIN			0
#define DUTY_CYCLE_MAX_SAFE		(DUTY_CYCLE_MAX - DUTY_CYCLE_MIN_SAFE) // 90% for safe margin
#define DUTY_CYCLE_MIN_SAFE		(DUTY_CYCLE_MAX/10) // 10% for safe margin

#define OFF 		0
#define NORMAL 		1
#define INVERTED 	2

struct BLDC_pwm
{
  unsigned int duty_cycle_normal;
  unsigned int duty_cycle_inverted;
  unsigned int phase_a;
  unsigned int phase_b;
  unsigned int phase_c;
};

extern struct BLDC_pwm bldc_pwm;

void pwm_init (void);
void pwm_set_duty_cycle (unsigned int value);
void pwm_update_duty_cycle (void);

#endif
