#include "uart_commands.h"

#include <stdio.h>

/* DeviceCommand oneof field numbers from protocol/device.proto. */
#define DEVICE_COMMAND_SET_MAX_TEMP 1U
#define DEVICE_COMMAND_SET_MIN_TEMP 2U
#define DEVICE_COMMAND_SET_CURRENT_TIME 3U

#define PROTOBUF_WIRE_VARINT 0U
#define PROTOBUF_WIRE_LENGTH_DELIMITED 2U

static void log_rx_byte(uart_commands_t *commands, uint8_t byte);
static void process_rx_byte(uart_commands_t *commands, uint8_t byte);
static void reset_parser(uart_commands_parser_t *parser);
static void finish_command(uart_commands_t *commands);
static bool decode_scalar_value(const uint8_t *payload, uint16_t length, uint64_t *value);
static bool decode_varint(const uint8_t *data, uint16_t length,
		uint16_t *position, uint64_t *value);
static bool decode_temperature(uint64_t encoded, int16_t *temperature);
static void print_temperature(const char *command, int16_t temperature);

void uart_commands_init(uart_commands_t *commands, UART_HandleTypeDef *huart)
{
	if (commands == NULL)
	{
		return;
	}

	commands->rx.huart = huart;
	commands->rx.byte = 0U;
	commands->rx.head = 0U;
	commands->rx.tail = 0U;
	reset_parser(&commands->parser);

	commands->values.min_temp_updated = false;
	commands->values.max_temp_updated = false;
	commands->values.current_time_updated = false;

	commands->stats.byte_count = 0U;
	commands->stats.error_count = 0U;
	commands->stats.overflow_count = 0U;
	commands->stats.decode_error_count = 0U;
	commands->stats.printed_error_count = 0U;
	commands->stats.printed_overflow_count = 0U;
	commands->stats.printed_decode_error_count = 0U;
}

uint8_t *uart_commands_rx_byte_ptr(uart_commands_t *commands)
{
	if (commands == NULL)
	{
		return NULL;
	}

	return &commands->rx.byte;
}

void uart_commands_on_rx_complete(uart_commands_t *commands)
{
	if (commands == NULL)
	{
		return;
	}

	commands->stats.byte_count++;
	log_rx_byte(commands, commands->rx.byte);

	if (commands->rx.huart == NULL ||
		HAL_UART_Receive_IT(commands->rx.huart, &commands->rx.byte, 1U) != HAL_OK)
	{
		commands->stats.error_count++;
	}
}

void uart_commands_poll(uart_commands_t *commands)
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

	if (commands->stats.printed_error_count != commands->stats.error_count)
	{
		commands->stats.printed_error_count = commands->stats.error_count;
		printf("[USART1] RX errors: %lu\r\n",
				(unsigned long)commands->stats.error_count);
	}

	if (commands->stats.printed_overflow_count != commands->stats.overflow_count)
	{
		commands->stats.printed_overflow_count = commands->stats.overflow_count;
		printf("[USART1] RX buffer overflows: %lu\r\n",
				(unsigned long)commands->stats.overflow_count);
	}

	if (commands->stats.printed_decode_error_count != commands->stats.decode_error_count)
	{
		commands->stats.printed_decode_error_count = commands->stats.decode_error_count;
		printf("[USART1] Protobuf decode errors: %lu\r\n",
				(unsigned long)commands->stats.decode_error_count);
	}
}

static void log_rx_byte(uart_commands_t *commands, uint8_t byte)
{
	uint16_t next_head = (uint16_t)(commands->rx.head + 1U);

	if (next_head >= UART_COMMANDS_RX_LOG_SIZE)
	{
		next_head = 0U;
	}

	if (next_head == commands->rx.tail)
	{
		commands->stats.overflow_count++;
		return;
	}

	commands->rx.buffer[commands->rx.head] = byte;
	commands->rx.head = next_head;
}

static void process_rx_byte(uart_commands_t *commands, uint8_t byte)
{
	uart_commands_parser_t *parser = &commands->parser;

	switch (parser->state)
	{
	case UART_COMMANDS_WAIT_TAG:
		/* All current oneof tags are one-byte, length-delimited keys. */
		if ((byte & 0x07U) != PROTOBUF_WIRE_LENGTH_DELIMITED)
		{
			commands->stats.decode_error_count++;
			return;
		}

		parser->command_field = (uint8_t)(byte >> 3U);
		if (parser->command_field < DEVICE_COMMAND_SET_MAX_TEMP ||
			parser->command_field > DEVICE_COMMAND_SET_CURRENT_TIME)
		{
			commands->stats.decode_error_count++;
			reset_parser(parser);
			return;
		}

		parser->state = UART_COMMANDS_READ_LENGTH;
		break;

	case UART_COMMANDS_READ_LENGTH:
		parser->payload_length |= (uint16_t)(byte & 0x7FU) << parser->length_shift;
		if ((byte & 0x80U) != 0U)
		{
			parser->length_shift = (uint8_t)(parser->length_shift + 7U);
			if (parser->length_shift >= 14U)
			{
				commands->stats.decode_error_count++;
				reset_parser(parser);
			}
			break;
		}

		if (parser->payload_length > UART_COMMANDS_PAYLOAD_SIZE)
		{
			commands->stats.decode_error_count++;
			reset_parser(parser);
			break;
		}

		if (parser->payload_length == 0U)
		{
			/* Proto3 omits a scalar field whose value is zero. */
			finish_command(commands);
		}
		else
		{
			parser->state = UART_COMMANDS_READ_PAYLOAD;
		}
		break;

	case UART_COMMANDS_READ_PAYLOAD:
		parser->payload[parser->payload_pos++] = byte;
		if (parser->payload_pos == parser->payload_length)
		{
			finish_command(commands);
		}
		break;

	default:
		commands->stats.decode_error_count++;
		reset_parser(parser);
		break;
	}
}

static void reset_parser(uart_commands_parser_t *parser)
{
	parser->state = UART_COMMANDS_WAIT_TAG;
	parser->command_field = 0U;
	parser->length_shift = 0U;
	parser->payload_length = 0U;
	parser->payload_pos = 0U;
}

static void finish_command(uart_commands_t *commands)
{
	uint64_t value = 0U;
	int16_t temperature;
	uint8_t command_field = commands->parser.command_field;
	bool decoded = decode_scalar_value(commands->parser.payload,
			commands->parser.payload_length, &value);

	reset_parser(&commands->parser);

	if (!decoded)
	{
		commands->stats.decode_error_count++;
		return;
	}

	switch (command_field)
	{
	case DEVICE_COMMAND_SET_MAX_TEMP:
		if (!decode_temperature(value, &temperature))
		{
			commands->stats.decode_error_count++;
			return;
		}
		commands->values.max_temp = temperature;
		commands->values.max_temp_updated = true;
		print_temperature("SetMaxTemp", temperature);
		break;

	case DEVICE_COMMAND_SET_MIN_TEMP:
		if (!decode_temperature(value, &temperature))
		{
			commands->stats.decode_error_count++;
			return;
		}
		commands->values.min_temp = temperature;
		commands->values.min_temp_updated = true;
		print_temperature("SetMinTemp", temperature);
		break;

	case DEVICE_COMMAND_SET_CURRENT_TIME:
		commands->values.current_time_ms = value;
		commands->values.current_time_updated = true;
		printf("[USART1] SetCurrentTime received\r\n");
		break;

	default:
		commands->stats.decode_error_count++;
		break;
	}
}

static bool decode_scalar_value(const uint8_t *payload, uint16_t length, uint64_t *value)
{
	uint16_t position = 0U;
	uint64_t key;

	if (value == NULL)
	{
		return false;
	}

	*value = 0U;
	if (length == 0U)
	{
		return true;
	}

	if (payload == NULL || !decode_varint(payload, length, &position, &key))
	{
		return false;
	}

	if ((key >> 3U) != 1U || (key & 0x07U) != PROTOBUF_WIRE_VARINT)
	{
		return false;
	}

	if (!decode_varint(payload, length, &position, value))
	{
		return false;
	}

	return position == length;
}

static bool decode_varint(const uint8_t *data, uint16_t length,
		uint16_t *position, uint64_t *value)
{
	uint8_t index;
	uint64_t result = 0U;

	if (data == NULL || position == NULL || value == NULL)
	{
		return false;
	}

	for (index = 0U; index < 10U && *position < length; index++)
	{
		uint8_t byte = data[(*position)++];

		/* A uint64 varint may use only bit zero of its tenth byte. */
		if (index == 9U && (byte & 0xFEU) != 0U)
		{
			return false;
		}

		result |= (uint64_t)(byte & 0x7FU) << (index * 7U);
		if ((byte & 0x80U) == 0U)
		{
			*value = result;
			return true;
		}
	}

	return false;
}

static bool decode_temperature(uint64_t encoded, int16_t *temperature)
{
	int32_t decoded;

	/* sint32 uses ZigZag encoding. Every valid int16 maps into 0..65535. */
	if (temperature == NULL || encoded > UINT16_MAX)
	{
		return false;
	}

	decoded = (int32_t)(encoded >> 1U);
	if ((encoded & 1U) != 0U)
	{
		decoded = -decoded - 1;
	}

	if (decoded < INT16_MIN || decoded > INT16_MAX)
	{
		return false;
	}

	*temperature = (int16_t)decoded;
	return true;
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
