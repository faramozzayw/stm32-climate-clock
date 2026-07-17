#include "command_receiver/uart_command_receiver.h"

#include <stdio.h>

#include "command_receiver/device_command_decoder.h"

static void log_rx_byte(uart_command_receiver_t *commands, uint8_t byte);
static void process_rx_byte(uart_command_receiver_t *commands, uint8_t byte);
static void apply_decoded_command(uart_command_receiver_t *commands,
		const decoded_device_command_t *command);
static void print_temperature(const char *command, int16_t temperature);

void uart_command_receiver_init(uart_command_receiver_t *commands,
		UART_HandleTypeDef *huart)
{
	if (commands == NULL)
	{
		return;
	}

	commands->rx.huart = huart;
	commands->rx.byte = 0U;
	commands->rx.head = 0U;
	commands->rx.tail = 0U;
	uart_frame_parser_init(&commands->frame_parser);

	commands->values.min_temp_updated = false;
	commands->values.max_temp_updated = false;
	commands->values.current_time_updated = false;

	commands->stats.rx_byte_count = 0U;
	commands->stats.rx_rearm_error_count = 0U;
	commands->stats.rx_overflow_count = 0U;
	commands->stats.frame_error_count = 0U;
	commands->stats.protobuf_decode_error_count = 0U;
	commands->stats.last_reported_rx_rearm_error_count = 0U;
	commands->stats.last_reported_rx_overflow_count = 0U;
	commands->stats.last_reported_frame_error_count = 0U;
	commands->stats.last_reported_protobuf_decode_error_count = 0U;
}

uint8_t *uart_command_receiver_rx_byte_ptr(uart_command_receiver_t *commands)
{
	if (commands == NULL)
	{
		return NULL;
	}

	return &commands->rx.byte;
}

void uart_command_receiver_on_rx_complete(uart_command_receiver_t *commands)
{
	if (commands == NULL)
	{
		return;
	}

	commands->stats.rx_byte_count++;
	log_rx_byte(commands, commands->rx.byte);

	if (commands->rx.huart == NULL ||
		HAL_UART_Receive_IT(commands->rx.huart, &commands->rx.byte, 1U) != HAL_OK)
	{
		commands->stats.rx_rearm_error_count++;
	}
}

void uart_command_receiver_poll(uart_command_receiver_t *commands)
{
	if (commands == NULL)
	{
		return;
	}

	while (commands->rx.tail != commands->rx.head)
	{
		uint8_t byte = commands->rx.buffer[commands->rx.tail];

		commands->rx.tail = (uint16_t)(commands->rx.tail + 1U);
		if (commands->rx.tail >= UART_COMMANDS_RX_LOG_SIZE)
		{
			commands->rx.tail = 0U;
		}

		process_rx_byte(commands, byte);
	}

	if (commands->stats.last_reported_rx_rearm_error_count !=
		commands->stats.rx_rearm_error_count)
	{
		commands->stats.last_reported_rx_rearm_error_count =
				commands->stats.rx_rearm_error_count;
		printf("[USART1] RX errors: %lu\r\n",
				(unsigned long)commands->stats.rx_rearm_error_count);
	}

	if (commands->stats.last_reported_rx_overflow_count !=
		commands->stats.rx_overflow_count)
	{
		commands->stats.last_reported_rx_overflow_count =
				commands->stats.rx_overflow_count;
		printf("[USART1] RX buffer overflows: %lu\r\n",
				(unsigned long)commands->stats.rx_overflow_count);
	}

	if (commands->stats.last_reported_frame_error_count !=
		commands->stats.frame_error_count)
	{
		commands->stats.last_reported_frame_error_count =
				commands->stats.frame_error_count;
		printf("[USART1] UART frame errors: %lu\r\n",
				(unsigned long)commands->stats.frame_error_count);
	}

	if (commands->stats.last_reported_protobuf_decode_error_count !=
		commands->stats.protobuf_decode_error_count)
	{
		commands->stats.last_reported_protobuf_decode_error_count =
				commands->stats.protobuf_decode_error_count;
		printf("[USART1] Protobuf decode errors: %lu\r\n",
				(unsigned long)commands->stats.protobuf_decode_error_count);
	}
}

static void log_rx_byte(uart_command_receiver_t *commands, uint8_t byte)
{
	uint16_t next_head = (uint16_t)(commands->rx.head + 1U);

	if (next_head >= UART_COMMANDS_RX_LOG_SIZE)
	{
		next_head = 0U;
	}

	if (next_head == commands->rx.tail)
	{
		commands->stats.rx_overflow_count++;
		return;
	}

	commands->rx.buffer[commands->rx.head] = byte;
	commands->rx.head = next_head;
}

static void process_rx_byte(uart_command_receiver_t *commands, uint8_t byte)
{
	uart_frame_view_t frame;
	uart_frame_result_t frame_result = uart_frame_parser_process(
			&commands->frame_parser, byte, &frame);

	if (frame_result == UART_FRAME_ERROR)
	{
		commands->stats.frame_error_count++;
	}
	else if (frame_result == UART_FRAME_COMPLETE)
	{
		decoded_device_command_t decoded_command;

		if (device_command_decode(frame.payload, frame.payload_length,
				&decoded_command) != DEVICE_COMMAND_DECODE_OK)
		{
			commands->stats.protobuf_decode_error_count++;
		}
		else
		{
			apply_decoded_command(commands, &decoded_command);
		}
	}
}

static void apply_decoded_command(uart_command_receiver_t *commands,
		const decoded_device_command_t *command)
{
	switch (command->type)
	{
	case DEVICE_COMMAND_SET_MAX_TEMP:
		commands->values.max_temp = command->value.temperature;
		commands->values.max_temp_updated = true;
		print_temperature("SetMaxTemp", commands->values.max_temp);
		break;

	case DEVICE_COMMAND_SET_MIN_TEMP:
		commands->values.min_temp = command->value.temperature;
		commands->values.min_temp_updated = true;
		print_temperature("SetMinTemp", commands->values.min_temp);
		break;

	case DEVICE_COMMAND_SET_CURRENT_TIME:
		commands->values.current_time_ms = command->value.current_time_ms;
		commands->values.current_time_updated = true;
		printf("[USART1] SetCurrentTime received\r\n");
		break;

	default:
		commands->stats.protobuf_decode_error_count++;
		break;
	}
}

static void print_temperature(const char *command, int16_t temperature)
{
	int32_t signed_value = temperature;
	uint32_t magnitude = (uint32_t)(signed_value < 0 ? -signed_value : signed_value);

	printf("[USART1] %s = %s%lu.%lu C\r\n",
			command,
			signed_value < 0 ? "-" : "",
			(unsigned long)(magnitude / 10U),
			(unsigned long)(magnitude % 10U));
}
