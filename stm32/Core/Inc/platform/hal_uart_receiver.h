#ifndef INC_PLATFORM_HAL_UART_RECEIVER_H_
#define INC_PLATFORM_HAL_UART_RECEIVER_H_

#include <stdbool.h>
#include <stdint.h>

#include "command_receiver/uart_command_receiver.h"
#include "usart.h"

typedef struct
{
	UART_HandleTypeDef *uart;
	uart_command_receiver_t *receiver;
	uint8_t pending_byte;
	uint32_t rearm_error_count;
	uint32_t last_reported_rearm_error_count;
} hal_uart_receiver_t;

bool hal_uart_receiver_init(
	hal_uart_receiver_t *adapter,
	UART_HandleTypeDef *uart,
	uart_command_receiver_t *receiver);

bool hal_uart_receiver_start(hal_uart_receiver_t *adapter);

void hal_uart_receiver_on_complete(
	hal_uart_receiver_t *adapter,
	UART_HandleTypeDef *completed_uart);

void hal_uart_receiver_report_errors(hal_uart_receiver_t *adapter);

#endif /* INC_PLATFORM_HAL_UART_RECEIVER_H_ */
