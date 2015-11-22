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
  if (counter > 10) // 1 second
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

void initialize (void)
{
  gpio_init ();
  
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

  while (1) ;
}
