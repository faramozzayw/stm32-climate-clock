#ifndef INC_UART_COMMANDS_H_
#define INC_UART_COMMANDS_H_

#include <stdbool.h>
#include <stdint.h>
#include "usart.h"

#define UART_COMMANDS_RX_LOG_SIZE 128U
#define UART_COMMANDS_PAYLOAD_SIZE 16U

typedef struct
{
	UART_HandleTypeDef *huart;
	uint8_t byte;
	uint8_t buffer[UART_COMMANDS_RX_LOG_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} uart_commands_rx_t;

typedef enum
{
	UART_COMMANDS_WAIT_TAG = 0,
	UART_COMMANDS_READ_LENGTH,
	UART_COMMANDS_READ_PAYLOAD,
} uart_commands_parser_state_t;

typedef struct
{
	uart_commands_parser_state_t state;
	uint8_t command_field;
	uint8_t length_shift;
	uint16_t payload_length;
	uint16_t payload_pos;
	uint8_t payload[UART_COMMANDS_PAYLOAD_SIZE];
} uart_commands_parser_t;

typedef struct
{
	uint32_t byte_count;
	uint32_t error_count;
	uint32_t overflow_count;
	uint32_t decode_error_count;
	uint32_t printed_error_count;
	uint32_t printed_overflow_count;
	uint32_t printed_decode_error_count;
} uart_commands_stats_t;

typedef struct
{
	int16_t min_temp;
	int16_t max_temp;
	uint64_t current_time_ms;
	bool min_temp_updated;
	bool max_temp_updated;
	bool current_time_updated;
} uart_commands_values_t;

typedef struct
{
	uart_commands_rx_t rx;
	uart_commands_parser_t parser;
	uart_commands_stats_t stats;
	uart_commands_values_t values;
} uart_commands_t;

void uart_commands_init(uart_commands_t *commands, UART_HandleTypeDef *huart);
void uart_commands_on_rx_complete(uart_commands_t *commands);
void uart_commands_poll(uart_commands_t *commands);
uint8_t *uart_commands_rx_byte_ptr(uart_commands_t *commands);

#endif /* INC_UART_COMMANDS_H_ */
