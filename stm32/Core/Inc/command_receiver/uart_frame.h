#ifndef INC_UART_FRAME_H_
#define INC_UART_FRAME_H_

#include <stdint.h>

#define UART_FRAME_MAGIC_1 0xA5U
#define UART_FRAME_MAGIC_2 0x5AU
#define UART_FRAME_MAX_PAYLOAD_SIZE 32U

typedef enum
{
	UART_FRAME_WAIT_MAGIC_1 = 0,
	UART_FRAME_WAIT_MAGIC_2,
	UART_FRAME_READ_LENGTH_LOW,
	UART_FRAME_READ_LENGTH_HIGH,
	UART_FRAME_READ_PAYLOAD,
	UART_FRAME_READ_CRC_LOW,
	UART_FRAME_READ_CRC_HIGH,
} uart_frame_parser_state_t;

typedef struct
{
	uart_frame_parser_state_t state;
	uint16_t payload_length;
	uint16_t payload_pos;
	uint16_t crc;
	uint16_t received_crc;
	uint8_t payload[UART_FRAME_MAX_PAYLOAD_SIZE];
} uart_frame_parser_t;

typedef struct
{
	const uint8_t *payload;
	uint16_t payload_length;
} uart_frame_view_t;

typedef enum
{
	UART_FRAME_IN_PROGRESS = 0,
	UART_FRAME_COMPLETE,
	UART_FRAME_ERROR,
} uart_frame_result_t;

void uart_frame_parser_init(uart_frame_parser_t *parser);
uart_frame_result_t uart_frame_parser_process(uart_frame_parser_t *parser,
		uint8_t byte, uart_frame_view_t *frame);

#endif /* INC_UART_FRAME_H_ */
