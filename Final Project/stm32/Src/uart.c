/*
 * uart.c
 *
 *  Created on: May 18, 2024
 *      Author: musa
 */


/* Includes ------------------------------------------------------------------*/
#include "Drivers/uart.h"

/* Wrapper Function that transmits a message via UART.
 * The function transmits the message in the form of a string,
 * so input has to be a string.
 * UART channel is always the same, the global handler huart1.
*/
void transmit_UART_message(uint8_t *buffer)
{
  uint16_t len = (uint16_t) strlen((char*) buffer);
  HAL_UART_Transmit(&huart1, buffer, len, HAL_MAX_DELAY);
}
