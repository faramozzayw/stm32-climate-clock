#ifndef INC_PLATFORM_HAL_UART_TRANSPORT_H_
#define INC_PLATFORM_HAL_UART_TRANSPORT_H_

#include <stdbool.h>
#include <stdint.h>

#include "usart.h"

typedef struct
{
	UART_HandleTypeDef *uart;
	uint32_t timeout_ms;
} hal_uart_transport_t;

bool hal_uart_transport_send(
	const hal_uart_transport_t *transport,
	const uint8_t *data,
	uint16_t length);

#endif /* INC_PLATFORM_HAL_UART_TRANSPORT_H_ */
