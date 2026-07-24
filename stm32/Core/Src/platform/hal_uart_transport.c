#include "platform/hal_uart_transport.h"

#include <stddef.h>

bool hal_uart_transport_send(
	const hal_uart_transport_t *transport,
	const uint8_t *data,
	uint16_t length)
{
	if ((transport == NULL) || (transport->uart == NULL) ||
		(data == NULL) || (length == 0U))
	{
		return false;
	}

	return HAL_UART_Transmit(
			   transport->uart,
			   data,
			   length,
			   transport->timeout_ms) == HAL_OK;
}
