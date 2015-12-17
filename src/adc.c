/*
 * Generic Electric Unicycle firmware
 *
 * Copyright (C) Casainho, 2015.
 *
 * Released under the GPL License, Version 3
 */

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "adc.h"

/*
 * Target: 1A
 * 1.2A --> 0.0462V --> 58 ADC value
 * 0.8A --> 0.0308V --> 39 ADC value
 *
 * 1A = 0.0385V
 * 12 bits ADC; 4096 steps
 * 3.3V / 4096 = 0.0008
 *
 */
#define ADC_WATCHDOG_HIGHTHRESHOLD 58
#define ADC_WATCHDOG_LOWTHRESHOLD 39

unsigned int adc_watchdog_highthreshold = ADC_WATCHDOG_HIGHTHRESHOLD;
unsigned int adc_watchdog_lowthreshold = ADC_WATCHDOG_LOWTHRESHOLD;

static unsigned int adc_value = 0;

void ADC1_IRQHandler(void)
{
  unsigned int adc_current_value;
  unsigned int middle_threshold;

  adc_current_value = adc_get_current_value ();

  // calc the window middle value
  middle_threshold = adc_watchdog_lowthreshold + ((adc_watchdog_highthreshold - adc_watchdog_lowthreshold) / 2);

  if (adc_get_current_value () > middle_threshold)
  {
    TIM_CtrlPWMOutputs (TIM1, DISABLE); //disable PWM signals
  }
  else
  {
    TIM_CtrlPWMOutputs (TIM1, ENABLE); //enable PWM signals
  }

  /* Clear ADC1 AWD pending interrupt bit */
  ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
}

void adc_init (void)
{
  /* ADCCLK = PCLK2/8 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div8);

  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_InitTypeDef DMA_InitStructure;
  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(ADC1->DR);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) &adc_value;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  ADC_InitTypeDef ADC_InitStructure;
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel7 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_55Cycles5);

  /* Configure high and low analog watchdog thresholds */
  ADC_AnalogWatchdogThresholdsConfig(ADC1, adc_watchdog_highthreshold, adc_watchdog_lowthreshold);
  /* Configure channel14 as the single analog watchdog guarded channel */
  ADC_AnalogWatchdogSingleChannelConfig(ADC1, ADC_Channel_7);
  /* Enable analog watchdog on one regular channel */
  ADC_AnalogWatchdogCmd(ADC1, ADC_AnalogWatchdog_SingleRegEnable);

  /* Enable AWD interrupt */
  ADC_ITConfig(ADC1, ADC_IT_AWD, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}

unsigned int adc_get_current_value (void)
{
  return adc_value;
}

void start_adc_max_current_management (void)
{
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void stop_adc_max_current_management (void)
{
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConvCmd(ADC1, DISABLE);
}
