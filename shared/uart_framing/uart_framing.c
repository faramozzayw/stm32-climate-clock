#include "uart_framing.h"

#include <stddef.h>

#define UART_FRAME_CRC_INITIAL 0xFFFFU
#define UART_FRAME_CRC_POLYNOMIAL 0x1021U

static uint16_t crc16_ccitt_update(uint16_t crc, uint8_t byte);
static uart_frame_result_t reject_frame(uart_frame_parser_t *parser);

bool uart_frame_encode(
	const uint8_t *payload,
	uint16_t payload_length,
	uint8_t *frame,
	uint16_t frame_capacity,
	uint16_t *frame_length)
{
	uint16_t crc = UART_FRAME_CRC_INITIAL;
	uint16_t required_capacity;
	uint16_t index;

	if ((payload == NULL) || (frame == NULL) || (frame_length == NULL) ||
		(payload_length == 0U) ||
		(payload_length > UART_FRAME_MAX_PAYLOAD_SIZE))
	{
		return false;
	}

	required_capacity =
		(uint16_t)(payload_length + UART_FRAME_OVERHEAD_SIZE);
	if (frame_capacity < required_capacity)
	{
		return false;
	}

	frame[0] = UART_FRAME_MAGIC_1;
	frame[1] = UART_FRAME_MAGIC_2;
	frame[2] = (uint8_t)(payload_length & 0xFFU);
	frame[3] = (uint8_t)(payload_length >> 8U);

	crc = crc16_ccitt_update(crc, frame[2]);
	crc = crc16_ccitt_update(crc, frame[3]);

	for (index = 0U; index < payload_length; index++)
	{
		frame[4U + index] = payload[index];
		crc = crc16_ccitt_update(crc, payload[index]);
	}

	frame[4U + payload_length] = (uint8_t)(crc & 0xFFU);
	frame[5U + payload_length] = (uint8_t)(crc >> 8U);
	*frame_length = required_capacity;

	return true;
}

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
	if ((parser == NULL) || (frame == NULL))
	{
		return UART_FRAME_RESULT_ERROR;
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
		if ((parser->payload_length == 0U) ||
			(parser->payload_length > UART_FRAME_MAX_PAYLOAD_SIZE))
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
		return UART_FRAME_RESULT_COMPLETE;

	default:
		return reject_frame(parser);
	}

	return UART_FRAME_RESULT_IN_PROGRESS;
}

static uart_frame_result_t reject_frame(uart_frame_parser_t *parser)
{
	uart_frame_parser_init(parser);
	return UART_FRAME_RESULT_ERROR;
}

/**
 * @brief Add one byte to an incremental CRC-16/CCITT-FALSE calculation.
 *
 * UART can deliver corrupted bytes without indicating which frame they belong
 * to. The CRC lets the receiver detect changes to the encoded payload length
 * or payload and reject the frame before attempting to decode it.
 *
 * Start with UART_FRAME_CRC_INITIAL, then pass the returned CRC back into this
 * function for each length and payload byte in transmission order. The
 * calculation uses polynomial 0x1021 without reflection or a final XOR.
 *
 * @param crc CRC accumulated from the preceding bytes.
 * @param byte Next byte to include in the calculation.
 * @return Updated CRC containing the supplied byte.
 */
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
