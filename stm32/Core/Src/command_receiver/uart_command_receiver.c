#include "command_receiver/uart_command_receiver.h"

#include <stdio.h>

#include "command_receiver/device_message_decoder.h"

static void log_rx_byte(uart_command_receiver_t *commands, uint8_t byte);
static void process_rx_byte(uart_command_receiver_t *commands, uint8_t byte);
static void apply_decoded_message(uart_command_receiver_t *commands,
	const decoded_device_message_t *message);
static void print_temperature(const char *command, int16_t temperature);

void uart_command_receiver_init(
	uart_command_receiver_t *commands,
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
	commands->values.temperature_unit = DEVICE_TEMPERATURE_UNIT_CELSIUS;
	commands->values.temperature_unit_updated = false;
	commands->values.ble_connection_state = BLE_CONNECTION_STATE_DISCONNECTED;

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

	if (frame_result == UART_FRAME_RESULT_ERROR)
	{
		commands->stats.frame_error_count++;
	}
	else if (frame_result == UART_FRAME_RESULT_COMPLETE)
	{
		decoded_device_message_t decoded_message;

		if (device_message_decode(frame.payload, frame.payload_length,
				&decoded_message) != DEVICE_MESSAGE_DECODE_OK)
		{
			commands->stats.protobuf_decode_error_count++;
		}
		else
		{
			apply_decoded_message(commands, &decoded_message);
		}
	}
}

static void apply_decoded_message(uart_command_receiver_t *commands,
	const decoded_device_message_t *message)
{
	switch (message->type)
	{
	case DEVICE_MESSAGE_SET_MAX_TEMP:
		commands->values.max_temp = message->value.temperature;
		commands->values.max_temp_updated = true;
		print_temperature("SetMaxTemp", commands->values.max_temp);
		break;

	case DEVICE_MESSAGE_SET_MIN_TEMP:
		commands->values.min_temp = message->value.temperature;
		commands->values.min_temp_updated = true;
		print_temperature("SetMinTemp", commands->values.min_temp);
		break;

	case DEVICE_MESSAGE_SET_CURRENT_TIME:
		commands->values.current_time_ms = message->value.current_time_ms;
		commands->values.current_time_updated = true;
		printf("[USART1] SetCurrentTime received\r\n");
		break;

	case DEVICE_MESSAGE_SET_TEMPERATURE_UNIT:
		commands->values.temperature_unit = message->value.temperature_unit;
		commands->values.temperature_unit_updated = true;
		printf("[USART1] Temperature unit = %c\r\n",
			commands->values.temperature_unit == DEVICE_TEMPERATURE_UNIT_FAHRENHEIT
				? 'F'
				: 'C');
		break;

	case DEVICE_MESSAGE_BLE_CONNECTION_STATE:
		commands->values.ble_connection_state =
			message->value.ble_connection_state;

		switch (commands->values.ble_connection_state)
		{
		case BLE_CONNECTION_STATE_DISCONNECTED:
			printf("[USART1] BLE disconnected\r\n");
			break;

		case BLE_CONNECTION_STATE_CONNECTING:
			printf("[USART1] BLE connecting\r\n");
			break;

		case BLE_CONNECTION_STATE_CONNECTED:
			printf("[USART1] BLE connected\r\n");
			break;

		case BLE_CONNECTION_STATE_DISCONNECTING:
			printf("[USART1] BLE disconnecting\r\n");
			break;
		}
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
