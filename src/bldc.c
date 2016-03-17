/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "gpio.h"
#include "bldc.h"
#include "pwm.h"

#define HALL_SENSORS_MASK ((1 << 0) | (1 << 1) | (1 << 2))

extern GPIO_InitTypeDef GPIO_InitStructure;

unsigned int bldc_machine_state = BLDC_NORMAL;
static unsigned int _direction = RIGHT;

// We start with the sine waves with 120º of each other
static unsigned int svm_table_index_a = 0;
static unsigned int svm_table_index_b = 12;
static unsigned int svm_table_index_c = 24;

struct Bldc_phase_state bldc_phase_state;

// Space Vector Modulation PWMs values, please read this blog message:
// http://www.berryjam.eu/2015/04/driving-bldc-gimbals-at-super-slow-speeds-with-arduino/
// Please see file: BLDC_SPWM_Lookup_tables.ods
unsigned int svm_table [36] =
{
  1152,
  1418,
  1677,
  1919,
  1984,
  2024,
  2038,
  2024,
  1984,
  1919,
  1984,
  2024,
  2038,
  2024,
  1984,
  1919,
  1677,
  1418,
  1152,
  885,
  626,
  384,
  319,
  279,
  265,
  279,
  319,
  384,
  319,
  279,
  265,
  279,
  319,
  384,
  626,
  885
};

unsigned int inc_svm_table_index (unsigned int index)
{
  if (index < 35)
  {
    index++;
  }
  else
  {
    index = 0;
  }
}

void bldc_svm_tick (void)
{
  float pwm_scale_factor;

  // Update the index values
  svm_table_index_a = inc_svm_table_index (svm_table_index_a);
  svm_table_index_a = inc_svm_table_index (svm_table_index_a);
  svm_table_index_a = inc_svm_table_index (svm_table_index_a);
  svm_table_index_a = inc_svm_table_index (svm_table_index_a);
  svm_table_index_a = inc_svm_table_index (svm_table_index_a);
  svm_table_index_a = inc_svm_table_index (svm_table_index_a);

  svm_table_index_b = inc_svm_table_index (svm_table_index_b);
  svm_table_index_b = inc_svm_table_index (svm_table_index_b);
  svm_table_index_b = inc_svm_table_index (svm_table_index_b);
  svm_table_index_b = inc_svm_table_index (svm_table_index_b);
  svm_table_index_b = inc_svm_table_index (svm_table_index_b);
  svm_table_index_b = inc_svm_table_index (svm_table_index_b);

  svm_table_index_c = inc_svm_table_index (svm_table_index_c);
  svm_table_index_c = inc_svm_table_index (svm_table_index_c);
  svm_table_index_c = inc_svm_table_index (svm_table_index_c);
  svm_table_index_c = inc_svm_table_index (svm_table_index_c);
  svm_table_index_c = inc_svm_table_index (svm_table_index_c);
  svm_table_index_c = inc_svm_table_index (svm_table_index_c);

  // Scale and apply the duty cycle values
  pwm_scale_factor = pwm_get_duty_cycle () + 999;
  pwm_scale_factor = pwm_scale_factor / 1999.0 ;
  TIM_SetCompare3(TIM1, (svm_table[svm_table_index_a]) * pwm_scale_factor);
  TIM_SetCompare1(TIM1, (svm_table[svm_table_index_b]) * pwm_scale_factor);
  TIM_SetCompare2(TIM1, (svm_table[svm_table_index_c]) * pwm_scale_factor);
}

void commutation_AB (void)
{
  svm_table_index_a = 0;
  svm_table_index_b = 12;
  svm_table_index_c = 24;
}

void commutation_AC (void)
{
  svm_table_index_a = 6;
  svm_table_index_b = 18;
  svm_table_index_c = 30;
}

void commutation_BC (void)
{
  svm_table_index_a = 12;
  svm_table_index_b = 24;
  svm_table_index_c = 0;
}

void commutation_BA (void)
{
  svm_table_index_a = 18;
  svm_table_index_b = 30;
  svm_table_index_c = 6;
}

void commutation_CA (void)
{
  svm_table_index_a = 24;
  svm_table_index_b = 0;
  svm_table_index_c = 12;
}

void commutation_CB (void)
{
  svm_table_index_a = 30;
  svm_table_index_b = 6;
  svm_table_index_c = 18;
}

void commutation_disable (void)
{
  TIM_CtrlPWMOutputs (TIM1, DISABLE); // PWM Output Disable
}

unsigned int get_current_sector (void)
{
  static unsigned int hall_sensors = 0;
  unsigned int sector;
  unsigned int i;

  hall_sensors = (GPIO_ReadInputData (GPIOA) & (HALL_SENSORS_MASK)); // mask other pins

  // IDENTIFY the sector from hall sensors signals
  //
  //       cba
  //  00000001 == 1
  //  00000011 == 3
  //  00000010 == 2
  //  00000110 == 6
  //  00000100 == 4
  //  00000101 == 5

  //Halls sequence: 6, 5, 2, 3, 1, 4 (includes the increment of the next step)
  switch (hall_sensors)
  {
    case 1:
    sector = 5;
    break;

    case 2:
    sector = 2;
    break;

    case 3:
    sector = 3;
    break;

    case 4:
    sector = 1;
    break;

    case 5:
    sector = 4;
    break;

    case 6:
    sector = 6;
    break;

    default:
    break;
  }

  return sector;
}

void commutate (void)
{
  volatile unsigned int sector;

  sector = get_current_sector ();

  //Coils: AB, AC, BC, BA, CA, CB
  switch (sector)
  {
    case 1:
    commutation_AB ();
    break;

    case 2:
    commutation_AC ();
    break;

    case 3:
    commutation_BC ();
    break;

    case 4:
    commutation_BA ();
    break;

    case 5:
    commutation_CA ();
    break;

    case 6:
    commutation_CB ();
    break;

    default:
    commutation_disable ();
    break;
  }
}

void bldc_set_direction (unsigned int direction)
{
  _direction = direction;
}

unsigned int bldc_get_direction (void)
{
  return _direction;
}

void bldc_set_state (unsigned int state)
{
  bldc_machine_state = state;
}

unsigned int bldc_get_state (void)
{
  return bldc_machine_state;
}
