/*
 * UART.h
 *
 *  Created on: 3 June 2018
 *      Author: Jeffrey Redondo
 *  Description: I took the code given by STM32CubeMx and created a separate file UART.c
 */
#include "stm32l4xx_hal.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart4;
//------------UART_Init------------
// Initialize the UARTs for 115200 baud rate (clck 32MHz),
// 8 bit word length, no parity bits, one stop bit, FIFOs enabled
// Input: none
// Output: none
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART4_UART_Init(void);
