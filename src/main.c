/*
    Copyright (C) 2015 Joerg Hoener;

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

void SysTick_Handler(void) // runs every 10ms
{
  static unsigned int counter = 0;
  static unsigned int led_state_flag = 0;

  counter++;
  if (counter > 100) // 1 second
  {
    if (led_state_flag == 0)
    {
      /* Enable the LEDs */
      GPIO_ResetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9);
      GPIO_ResetBits(GPIOA, GPIO_Pin_15);

      led_state_flag = 1;
    }
    else
    {
      /* Disable the LEDs */
      GPIO_SetBits(GPIOB, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9);
      GPIO_SetBits(GPIOA, GPIO_Pin_15);

      led_state_flag = 0;
    }

    counter = 0;
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
  SetSysClockTo64(); //configure clock to 64 MHz (max possible speed)
  SystemCoreClockUpdate();
  gpio_init ();
  commutation_disable ();
  pwm_init ();
  hall_sensor_init ();

  /* Setup SysTick Timer for 10 millisecond interrupts, also enables Systick and Systick-Interrupt */
  if (SysTick_Config(SystemCoreClock / 100))
  {
    /* Capture error */
    while (1);
  }
}

int main(void)
{

  initialize();

  motor_set_duty_cycle (100); // 100 --> 10%
  motor_start();

  while (1) ;
}
