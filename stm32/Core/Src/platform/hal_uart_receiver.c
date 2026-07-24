#include "platform/hal_uart_receiver.h"

#include <stddef.h>
#include <stdio.h>

static bool hal_uart_receiver_rearm(hal_uart_receiver_t *adapter)
{
	if ((adapter == NULL) || (adapter->uart == NULL) ||
		(adapter->receiver == NULL))
	{
		return false;
	}

	if (HAL_UART_Receive_IT(
			adapter->uart,
			&adapter->pending_byte,
			1U) != HAL_OK)
	{
		adapter->rearm_error_count++;
		return false;
	}

	return true;
}

bool hal_uart_receiver_init(
	hal_uart_receiver_t *adapter,
	UART_HandleTypeDef *uart,
	uart_command_receiver_t *receiver)
{
	if ((adapter == NULL) || (uart == NULL) || (receiver == NULL))
	{
		return false;
	}

	adapter->uart = uart;
	adapter->receiver = receiver;
	adapter->pending_byte = 0U;
	adapter->rearm_error_count = 0U;
	adapter->last_reported_rearm_error_count = 0U;
	return true;
}

bool hal_uart_receiver_start(hal_uart_receiver_t *adapter)
{
	return hal_uart_receiver_rearm(adapter);
}

void hal_uart_receiver_on_complete(
	hal_uart_receiver_t *adapter,
	UART_HandleTypeDef *completed_uart)
{
	if ((adapter == NULL) || (completed_uart != adapter->uart) ||
		(adapter->receiver == NULL))
	{
		return;
	}

	uart_command_receiver_push_byte(
		adapter->receiver,
		adapter->pending_byte);
	hal_uart_receiver_rearm(adapter);
}

void hal_uart_receiver_report_errors(hal_uart_receiver_t *adapter)
{
	if ((adapter == NULL) ||
		(adapter->last_reported_rearm_error_count ==
			adapter->rearm_error_count))
	{
		return;
	}

	adapter->last_reported_rearm_error_count =
		adapter->rearm_error_count;
	printf("[USART1] RX errors: %lu\r\n",
		(unsigned long)adapter->rearm_error_count);
}
