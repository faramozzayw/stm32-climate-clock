#ifndef INC_UART_COMMAND_RECEIVER_H_
#define INC_UART_COMMAND_RECEIVER_H_

#include <stdbool.h>
#include <stdint.h>
#include "utils/byte_ring_buffer.h"
#include "command_receiver/device_message_decoder.h"
#include "command_receiver/uart_frame.h"

#define UART_COMMAND_RECEIVER_RX_CAPACITY 128U

typedef struct
{
	uint8_t storage[UART_COMMAND_RECEIVER_RX_CAPACITY];
	byte_ring_buffer_t bytes;
} uart_command_receiver_rx_t;

typedef struct
{
	uint32_t rx_byte_count;
	uint32_t rx_overflow_count;
	uint32_t frame_error_count;
	uint32_t protobuf_decode_error_count;
	uint32_t last_reported_rx_overflow_count;
	uint32_t last_reported_frame_error_count;
	uint32_t last_reported_protobuf_decode_error_count;
} uart_command_receiver_stats_t;

typedef struct
{
	int16_t min_temp;
	int16_t max_temp;
	uint64_t current_time_ms;
	device_temperature_unit_t temperature_unit;
	ble_connection_state_t ble_connection_state;
	bool min_temp_updated;
	bool max_temp_updated;
	bool current_time_updated;
	bool temperature_unit_updated;
} uart_command_receiver_values_t;

typedef struct
{
	uart_command_receiver_rx_t rx;
	uart_frame_parser_t frame_parser;
	uart_command_receiver_stats_t stats;
	uart_command_receiver_values_t values;
} uart_command_receiver_t;

void uart_command_receiver_init(uart_command_receiver_t *receiver);
void uart_command_receiver_push_byte(
	uart_command_receiver_t *receiver,
	uint8_t byte);
void uart_command_receiver_poll(uart_command_receiver_t *receiver);

#endif /* INC_UART_COMMAND_RECEIVER_H_ */
