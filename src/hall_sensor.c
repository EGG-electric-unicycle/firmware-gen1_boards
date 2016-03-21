/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
*/

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"
#include "main.h"
#include "bldc.h"

static uint16_t hall_sensors_time = 0;


volatile unsigned int sequential_signal = 0;

#define HALL_SENSORS_MASK ((1 << 0) | (1 << 1) | (1 << 2))

void TIM2_IRQHandler(void)
{
  static uint16_t timer = 0;

  if (TIM_GetFlagStatus (TIM2, TIM_FLAG_Trigger))
  {
    commutate ();

    /* Save current time between each hall sensor signal change */
    timer = (unsigned int) TIM_GetCapture1 (TIM2);

    // clear interrupt flag for this interrupt
    TIM_ClearITPendingBit (TIM2, TIM_IT_Trigger | TIM_IT_Update);

  }
  else if (TIM_GetFlagStatus (TIM2, TIM_FLAG_Update))
  {
    commutate ();

    timer = 50000; // TIM2 overflow value at 500ms

    // clear interrupt flag for this interrupt
    TIM_ClearITPendingBit (TIM2, TIM_IT_Update);
  }
}

unsigned int get_hall_sensors_10us (void)
{
  return hall_sensors_time;
}

void hall_sensor_init (void)
{ 
  // Enable clock for TIM2, used by hall sensors
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  /*
   * Calc for the max possible time between each hall sensor signal
   * Assuming max of 35 km/h speed of the wheel:
   * 1 rotation / second --> 46 impulses
   * 1 rotation / second --> 1 / 46 = 21.7ms each impulse
   * 1 rotation / second --> 1.12 * 60 * 60 = 4.032 km/h. Note: (1.12m = 14" wheel perimeter)
   * 35km/h --> 35 / 4.032 = 8.7
   * each impulse for 35km/h --> 21.7 / 8.7 = 2.5ms
   *
   * Each impulse need to be divided by 6 because we are using a space_vector_table that have 6 steps for each impulse
   * 2.5 / 6 = 417us
   *
   * Timer increment clock for capture signals can be 10us and so we will have a resolution of 400/10 = 40
   * for the max speed of 35km/h which should be good
   *
   */

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  // timer base configuration
  // 64MHz clock (PCLK1), 64MHz/640 = 1MHz --> 10us each increment of the counter/timer
  TIM_TimeBaseStructure.TIM_Prescaler = (640 - 1);
  TIM_TimeBaseStructure.TIM_Period = (50000 - 1); // max of 500ms or the Timer will overflow, (200ms about the walking speed of 5km/h)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  // enable hall sensor
  // T1F_ED will be connected to  HallSensors Inputs
  // TIM2_CH1, TIM2_CH2, TIM2_CH3
  TIM_SelectHallSensor(TIM2, ENABLE);

  // HallSensor event is delivered with signal TI1F_ED
  // (this is XOR of the three hall sensor lines)
  // Signal TI1F_ED: falling and rising edge of the inputs is used
  TIM_SelectInputTrigger(TIM2, TIM_TS_TI1F_ED);

  // On every TI1F_ED event the counter is resetted and update is triggered
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);

  // Channel 1 in input capture mode
  // on every TCR edge (build from TI1F_ED which is a HallSensor edge)
  // the timervalue is copied into ccr register and a CCR1 Interrupt
  // TIM_IT_CC1 is fired
  TIM_ICInitTypeDef TIM_ICInitStructure;
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  // listen to T1, the  HallSensorEvent
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_TRC;
  // Div:1, every edge
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM2, &TIM_ICInitStructure);

  /* Enable the TIM2 Trigger and Update Interrupts Request */
  TIM_ITConfig(TIM2, (TIM_IT_Trigger | TIM_IT_Update), ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  /* Configure and enable TIM2 interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM2_HALL_SENSORS_PRIORITY;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM2, ENABLE);
}
