#include "stm32f10x.h"
#include "stdio.h"

void usart1_init(void)
{
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);

  /* Enable the USART1 */
  USART_Cmd(USART1, ENABLE);

  // turn off buffers, so IO occurs immediately
//  setvbuf(stdin, NULL, _IONBF, 0);
//  setvbuf(stdout, NULL, _IONBF, 0);
//  setvbuf(stderr, NULL, _IONBF, 0);
}

/**
  * @brief  Retargets the C library printf function to the USART (GNU)
  * @param  None
  * @retval None
  */
int __io_putchar(int ch)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */

  /* Loop until the transmission buffer is empty */
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

  USART_SendData(USART1, (uint8_t) ch);

  return(ch);
}

//******************************************************************************

/**
  * @brief  Retargets the C library scanf function to the USART (GNU)
  * @param  None
  * @retval None
  */
int __io_getchar(void)
{
  /* Place your implementation of fgetc here */
  /* e.g. read a character from the USART */

  /* Loop until the reception buffer is not empty */
  while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

  return((int)USART_ReceiveData(USART1));
}

//******************************************************************************

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while(1); /* Infinite loop */
}
#endif

//******************************************************************************
