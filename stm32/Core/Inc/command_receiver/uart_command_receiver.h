#ifndef INC_UART_COMMAND_RECEIVER_H_
#define INC_UART_COMMAND_RECEIVER_H_

#include <stdbool.h>
#include <stdint.h>
#include "command_receiver/device_message_decoder.h"
#include "command_receiver/uart_frame.h"
#include "usart.h"

#define UART_COMMANDS_RX_LOG_SIZE 128U

typedef struct
{
	UART_HandleTypeDef *huart;
	uint8_t byte;
	uint8_t buffer[UART_COMMANDS_RX_LOG_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} uart_command_receiver_rx_t;

typedef struct
{
	uint32_t rx_byte_count;
	uint32_t rx_rearm_error_count;
	uint32_t rx_overflow_count;
	uint32_t frame_error_count;
	uint32_t protobuf_decode_error_count;
	uint32_t last_reported_rx_rearm_error_count;
	uint32_t last_reported_rx_overflow_count;
	uint32_t last_reported_frame_error_count;
	uint32_t last_reported_protobuf_decode_error_count;
} uart_command_receiver_stats_t;

typedef struct
{
	int16_t min_temp;
	int16_t max_temp;
	uint64_t current_time_ms;
	ble_connection_state_t ble_connection_state;
	bool min_temp_updated;
	bool max_temp_updated;
	bool current_time_updated;
} uart_command_receiver_values_t;

typedef struct
{
	uart_command_receiver_rx_t rx;
	uart_frame_parser_t frame_parser;
	uart_command_receiver_stats_t stats;
	uart_command_receiver_values_t values;
} uart_command_receiver_t;

void uart_command_receiver_init(uart_command_receiver_t *receiver, UART_HandleTypeDef *huart);
void uart_command_receiver_on_rx_complete(uart_command_receiver_t *receiver);
void uart_command_receiver_poll(uart_command_receiver_t *receiver);
uint8_t *uart_command_receiver_rx_byte_ptr(uart_command_receiver_t *receiver);

#endif /* INC_UART_COMMAND_RECEIVER_H_ */
