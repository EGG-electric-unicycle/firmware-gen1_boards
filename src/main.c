// By Wolfgang Wieser, heavily based on:
// http://fun-tech.se/stm32/OlimexBlinky/mini.php


#include "stm32f10x.h"

#include "IMU/imu.h"

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);


int main(void)
{

   /*!< At this stage the microcontroller clock setting is already configured, 
        this is done through SystemInit() function which is called from startup
        file (startup_stm32f10x_xx.s) before to branch to application main.
        To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f10x.c file
      */     
       
   /* System clocks configuration ---------------------------------------------*/
   RCC_Configuration();

   /* NVIC configuration ------------------------------------------------------*/
   NVIC_Configuration();

   /* GPIO configuration ------------------------------------------------------*/
   GPIO_Configuration();
  
   /*if (!IMU_init(, IMU_FILTER_KALMAN)) {
      // indicate IMU error (maybe via LEDs)
   }*/
  

  
   int i=0;

   for(;;)
   {
      i++;
   }
}


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
    
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 and DMA2 clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);

  /* Enable ADC1, ADC2, ADC3 and GPIOC clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 |
                         RCC_APB2Periph_ADC3 | RCC_APB2Periph_GPIOC, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PC.02, PC.03 and PC.04 (ADC Channel12, ADC Channel13 and 
     ADC Channel14) as analog inputs */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  Configures Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure and enable ADC interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
