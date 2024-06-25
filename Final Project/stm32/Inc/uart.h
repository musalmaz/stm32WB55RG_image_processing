/*
 * uart.h
 *
 *  Created on: May 18, 2024
 *      Author: musa
 */

#ifndef INC_DRIVERS_UART_H_
#define INC_DRIVERS_UART_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"
#include <stdio.h>
#include <string.h>

/* External Variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;

/* Functions  ----------------------------------------------------------------*/
void transmit_UART_message(uint8_t *buffer);
#endif

