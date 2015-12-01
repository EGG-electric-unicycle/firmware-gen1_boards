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

GPIO_InitTypeDef GPIO_InitStructure;

#define HALL_SENSORS_MASK ((1 << 0) | (1 << 1) | (1 << 2))

void phase_a_h_on (void)
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void phase_a_h_off (void)
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void phase_a_l_on (void)
{
  GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

void phase_a_l_off (void)
{
  GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}


void phase_b_h_on (void)
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void phase_b_h_off (void)
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void phase_b_l_on (void)
{
  GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

void phase_b_l_off (void)
{
  GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}


void phase_c_h_on (void)
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void phase_c_h_off (void)
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void phase_c_l_on (void)
{
  GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void phase_c_l_off (void)
{
  GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}


void commutation_sector_1 (void)
{
  phase_a_h_on ();
  phase_a_l_off ();

  phase_b_h_off ();
  phase_b_l_on ();

  phase_c_h_off ();
  phase_c_l_off ();
}

void commutation_sector_2 (void)
{
  phase_a_h_on ();
  phase_a_l_off ();

  phase_b_h_off ();
  phase_b_l_off ();

  phase_c_h_off ();
  phase_c_l_on ();
}

void commutation_sector_3 (void)
{
  phase_a_h_off ();
  phase_a_l_off ();

  phase_b_h_on ();
  phase_b_l_off ();

  phase_c_h_off ();
  phase_c_l_on ();
}

void commutation_sector_4 (void)
{
  phase_a_h_off ();
  phase_a_l_on ();

  phase_b_h_on ();
  phase_b_l_off ();

  phase_c_h_off ();
  phase_c_l_off ();
}

void commutation_sector_5 (void)
{
  phase_a_h_off ();
  phase_a_l_on ();

  phase_b_h_off ();
  phase_b_l_off ();

  phase_c_h_on ();
  phase_c_l_off ();
}

void commutation_sector_6 (void)
{
  phase_a_h_off ();
  phase_a_l_off ();

  phase_b_h_off ();
  phase_b_l_on ();

  phase_c_h_on ();
  phase_c_l_off ();
}

void commutation_disable (void)
{
  phase_a_h_off ();
  phase_a_l_off ();

  phase_b_h_off ();
  phase_b_l_off ();

  phase_c_h_off ();
  phase_c_l_off ();
}

unsigned int get_current_sector (void)
{
  static unsigned int hall_sensors = 0;
  unsigned int i;

  static unsigned int table[6] =
  {
	 //       cba
      1, //  00000001 == 1
      3, //  00000011 == 3
      2, //  00000010 == 2
      6, //  00000110 == 6
      4, //  00000100 == 4
      5, //  00000101 == 5
  };

  hall_sensors = (GPIO_ReadInputData (GPIOA) & (HALL_SENSORS_MASK)); // mask other pins

  // go trough the table to identify the indice and calc the sector number
  for (i = 0; i < 6; i++)
  {
    if (table[i] == hall_sensors)
    {
      return (i + 1); // return the sector number
    }
  }

  return 0;
}

void commutate (void)
{
  volatile unsigned int sector;

  sector = get_current_sector ();

  switch (sector)
  {
    case 1:
    commutation_sector_1 ();
    break;

    case 2:
    commutation_sector_2 ();
    break;

    case 3:
    commutation_sector_3 ();
    break;

    case 4:
    commutation_sector_4 ();
    break;

    case 5:
    commutation_sector_5 ();
    break;

    case 6:
    commutation_sector_6 ();
    break;

    default:
    commutation_disable ();
    break;
  }
}

void commutation_sector (unsigned int sector)
{
  switch (sector)
  {
    case 1:
    commutation_sector_1 ();
    break;

    case 2:
    commutation_sector_2 ();
    break;

    case 3:
    commutation_sector_3 ();
    break;

    case 4:
    commutation_sector_4 ();
    break;

    case 5:
    commutation_sector_5 ();
    break;

    case 6:
    commutation_sector_6 ();
    break;

    default:
    commutation_disable ();
    break;
  }
}

unsigned int increment_sector (unsigned int sector)
{
  if (sector < 6)
  {
    sector++;
  }
  else // sector = 6
  {
    sector = 1;
  }

  return sector;
}

unsigned int decrement_sector (unsigned int sector)
{
  if (sector > 1)
  {
    sector--;
  }
  else // sector = 1
  {
    sector = 6;
  }

  return sector;
}
