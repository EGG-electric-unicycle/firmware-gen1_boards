/*
    Copyright (C) 2015 Joerg Hoener;
    Copyright (C) 2015 Jorge Pinto;

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "gpio.h"
#include "main.h"
#include "bldc.h"
#include "adc.h"
#include "pwm.h"

unsigned int machine_state = COAST;

void SetSysClockTo64(void);
void initialize (void);

void SysTick_Handler(void) // runs every 1ms
{
  static unsigned int c = 0;

  //pwm_manage (); // manage the pwm duty-cycle

//  if (c++ > 999)
//  {
//    commutate_sector ();
//    c = 0;
//  }
}

int main(void)
{
  static int value;

  initialize();

  motor_start ();

  while (1)
  {
    // control the motor speed and rotation direction using a potentiometer on PS_SIGNAL pin
    value = (adc_get_PS_signal_value () >> 2); // filter and the value is now 10 bits --> max 1023.
    value = value - 511; // now middle value is 0. Left half of pot turns motor left and vice-versa
    value = value * 1.953; // scale: 512 * 1.953 = 999.9
    motor_set_duty_cycle (value);

    switch (machine_state)
    {
      case COAST:

      break;

      case RUNNING:

      break;

      case OVER_CURRENT:
      buzzer_on();
      __disable_irq;
      while (1) ; //block!!
      break;

      default:
      break;
    }
  }
}

/*
 * Configure the system clock to be 64 MHz (this is the max clock speed possible using the High Speed Internal oscillator)
 */
void SetSysClockTo64(void)
{
  /* Enable Prefetch Buffer */
  FLASH->ACR |= FLASH_ACR_PRFTBE;

  /* Flash 2 wait state */
  FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
  FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

  /* HCLK = SYSCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

  /* PCLK2 = HCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

  /* PCLK1 = HCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

  /*  PLL configuration: PLLCLK = HSI/2 * 16 = 64 MHz (this is the max clock speed possible using HSI) */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
  RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL16);

  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
  }

  /* Select PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
  {
  }
}

void initialize (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Config PS_signal pin */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PS_SIGNAL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //blocks while the PS_SIGNAL is low (there is a switch to control this signal
  //this is used to block firmware from running if for example it do a short circuit on the H bridges.
  while (!GPIO_ReadInputDataBit(GPIOA, PS_SIGNAL)) ;

  SetSysClockTo64(); //configure clock to 64 MHz (max possible speed)
  SystemCoreClockUpdate();

  // This will affect the configuration for all interrupts
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //4 bits for preemp priority 0 bit for sub priority

  adc_init ();
  commutation_disable ();
  pwm_init ();
  gpio_init (); // configure pins just after PWM init
  hall_sensor_init ();

  /* Setup SysTick Timer for 1 millisecond interrupts, also enables Systick and Systick-Interrupt */
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    /* Capture error */
    while (1);
  }
}
