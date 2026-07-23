/*
 * retarget.c
 *
 *  Created on: Jul 5, 2026
 *      Author: faram
 */

#include "usart.h"
#include <unistd.h>

int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
}
