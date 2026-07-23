#include "command_receiver/uart_frame.h"

#include <stddef.h>

#define UART_FRAME_CRC_INITIAL 0xFFFFU
#define UART_FRAME_CRC_POLYNOMIAL 0x1021U

static uint16_t crc16_ccitt_update(uint16_t crc, uint8_t byte);
static uart_frame_result_t reject_frame(uart_frame_parser_t *parser);

void uart_frame_parser_init(uart_frame_parser_t *parser)
{
	if (parser == NULL)
	{
		return;
	}

	parser->state = UART_FRAME_WAIT_MAGIC_1;
	parser->payload_length = 0U;
	parser->payload_pos = 0U;
	parser->crc = UART_FRAME_CRC_INITIAL;
	parser->received_crc = 0U;
}

uart_frame_result_t uart_frame_parser_process(
	uart_frame_parser_t *parser,
	uint8_t byte,
	uart_frame_view_t *frame)
{
	if (parser == NULL || frame == NULL)
	{
		return UART_FRAME_ERROR;
	}

	frame->payload = NULL;
	frame->payload_length = 0U;

	switch (parser->state)
	{
	case UART_FRAME_WAIT_MAGIC_1:
		if (byte == UART_FRAME_MAGIC_1)
		{
			parser->state = UART_FRAME_WAIT_MAGIC_2;
		}
		break;

	case UART_FRAME_WAIT_MAGIC_2:
		if (byte == UART_FRAME_MAGIC_2)
		{
			parser->state = UART_FRAME_READ_LENGTH_LOW;
		}
		else if (byte != UART_FRAME_MAGIC_1)
		{
			parser->state = UART_FRAME_WAIT_MAGIC_1;
		}
		break;

	case UART_FRAME_READ_LENGTH_LOW:
		parser->payload_length = byte;
		parser->crc = crc16_ccitt_update(UART_FRAME_CRC_INITIAL, byte);
		parser->state = UART_FRAME_READ_LENGTH_HIGH;
		break;

	case UART_FRAME_READ_LENGTH_HIGH:
		parser->payload_length |= (uint16_t)byte << 8U;
		parser->crc = crc16_ccitt_update(parser->crc, byte);
		if (parser->payload_length == 0U ||
			parser->payload_length > UART_FRAME_MAX_PAYLOAD_SIZE)
		{
			return reject_frame(parser);
		}
		parser->state = UART_FRAME_READ_PAYLOAD;
		break;

	case UART_FRAME_READ_PAYLOAD:
		parser->payload[parser->payload_pos++] = byte;
		parser->crc = crc16_ccitt_update(parser->crc, byte);
		if (parser->payload_pos == parser->payload_length)
		{
			parser->state = UART_FRAME_READ_CRC_LOW;
		}
		break;

	case UART_FRAME_READ_CRC_LOW:
		parser->received_crc = byte;
		parser->state = UART_FRAME_READ_CRC_HIGH;
		break;

	case UART_FRAME_READ_CRC_HIGH:
		parser->received_crc |= (uint16_t)byte << 8U;
		if (parser->received_crc != parser->crc)
		{
			return reject_frame(parser);
		}

		frame->payload = parser->payload;
		frame->payload_length = parser->payload_length;
		uart_frame_parser_init(parser);
		return UART_FRAME_COMPLETE;

	default:
		return reject_frame(parser);
	}

	return UART_FRAME_IN_PROGRESS;
}

static uart_frame_result_t reject_frame(uart_frame_parser_t *parser)
{
	uart_frame_parser_init(parser);
	return UART_FRAME_ERROR;
}

static uint16_t crc16_ccitt_update(uint16_t crc, uint8_t byte)
{
	uint8_t bit;

	crc ^= (uint16_t)byte << 8U;
	for (bit = 0U; bit < 8U; bit++)
	{
		if ((crc & 0x8000U) != 0U)
		{
			crc = (uint16_t)((crc << 1U) ^ UART_FRAME_CRC_POLYNOMIAL);
		}
		else
		{
			crc <<= 1U;
		}
	}

	return crc;
}
