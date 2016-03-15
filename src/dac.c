/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2016.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x_rcc.h"
#include "stm32f10x_dac.h"

void dac_init (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DAC_InitTypeDef  DAC_InitStructure;

  //(+) Enable DAC APB1 clock to get write access to DAC registers
  //    using RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  //(+) Configure DAC_OUT1 (DAC_OUT1: PA4) in analog mode
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //(+) Configure the DAC channel using DAC_Init()
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  //(+) Enable the DAC channel using DAC_Cmd()
  DAC_Cmd(DAC_Channel_1, ENABLE);
}

void dac_set_value (unsigned int value)
{
  DAC_SetChannel1Data(DAC_Align_12b_R, ((uint16_t) value));
}
