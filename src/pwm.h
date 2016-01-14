/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#ifndef _PWM_H_
#define _PWM_H_

#define PWM_DUTY_CYCLE_STEP		1 // step value to increment/decrement duty cycle value

void pwm_init (void);
void pwm_set_duty_cycle (int value);
void pwm_update_duty_cycle (void);

#endif
