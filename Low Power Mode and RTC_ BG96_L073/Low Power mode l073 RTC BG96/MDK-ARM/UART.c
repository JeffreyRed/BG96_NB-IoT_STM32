/*
 * UART.c
 *
 *  Created on: 3 June 2018
 *      Author: Jeffrey Redondo
 *  Description: I took the code given by STM32CubeMx and created a separate file UART.c
 *  for initialization of UART and interrupt subroutine.
 */

/* Includes ------------------------------------------------------------------*/
//#include "gpio.h"
#include "UART.h"
#include "stm32l0xx_hal.h"

/*Variables*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;

extern uint8_t buffer[100];
extern int len;
extern uint8_t sw, Rx_indx,EVB_indx, Rx_data[2], Rx_Buffer[100], Transfer_cplt, EVB_Buffer[200],EVBRx_data[2];


/* USART1 init function */
void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART4 init function 
//void MX_USART4_UART_Init(void)
//{

//  huart4.Instance = USART4;
//  huart4.Init.BaudRate = 115200;
//  huart4.Init.WordLength = UART_WORDLENGTH_8B;
//  huart4.Init.StopBits = UART_STOPBITS_1;
//  huart4.Init.Parity = UART_PARITY_NONE;
//  huart4.Init.Mode = UART_MODE_TX_RX;
//  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
//  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
//  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
//  if (HAL_UART_Init(&huart4) != HAL_OK)
//  {
//    _Error_Handler(__FILE__, __LINE__);
//  }

//}
*/

/**
  * @brief  Interrupt subroutine. Everytime a byte is received, it is stored in a buffer
  *  
  * @param  None
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance==USART2){
		if (Rx_data[0]!=13){ //checking if enter
			Rx_Buffer[Rx_indx++] = Rx_data[0];
			HAL_UART_Transmit(&huart2,Rx_data,strlen(Rx_data),100);
			HAL_UART_Receive_IT(&huart2,Rx_data,1);	
		} else {
			HAL_UART_Transmit(&huart2,"\r\n",4,100);	
			HAL_UART_Transmit(&huart1,Rx_Buffer,Rx_indx,1000);
			HAL_UART_Transmit(&huart1,"\r\n",4,1000);
			Rx_indx=0;
			HAL_UART_Receive_IT(&huart2,Rx_data,1);
		}	
	}	
	else if (huart->Instance==USART1){
		EVB_Buffer[EVB_indx++] = EVBRx_data[0];
		HAL_UART_Receive_IT(&huart1,EVBRx_data,1);		
	}
//		else if (huart->Instance==USART1){
//				EVB_Buffer[EVB_indx++] = EVBRx_data[0];
//					if((EVB_Buffer[EVB_indx-2]==0x4F)&&(EVBRx_data[0]==0x4B)){
//						HAL_UART_Transmit(&huart2,EVB_Buffer,EVB_indx,1000);
//						HAL_UART_Transmit(&huart2,"\r\n",2,100);
//						for(int j=0;j<EVB_indx;j++){
//						EVB_Buffer[j]=0x00;
//						}
//						EVB_indx=0;
//						sw=0;
//					}			
//			HAL_UART_Receive_IT(&huart1,EVBRx_data,1);		
//			}	
}