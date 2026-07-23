#ifndef SHARED_UART_FRAMING_H_
#define SHARED_UART_FRAMING_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define UART_FRAME_MAGIC_1 0xA5U
#define UART_FRAME_MAGIC_2 0x5AU
#define UART_FRAME_MAX_PAYLOAD_SIZE 32U
#define UART_FRAME_OVERHEAD_SIZE 6U
#define UART_FRAME_MAX_SIZE \
	(UART_FRAME_MAX_PAYLOAD_SIZE + UART_FRAME_OVERHEAD_SIZE)

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
		UART_FRAME_RESULT_IN_PROGRESS = 0,
		UART_FRAME_RESULT_COMPLETE,
		UART_FRAME_RESULT_ERROR,
	} uart_frame_result_t;

	/**
	 * @brief Encode a payload with UART framing and CRC.
	 *
	 * @param payload Payload to encode.
	 * @param payload_length Number of payload bytes.
	 * @param frame Destination for the encoded frame.
	 * @param frame_capacity Capacity of the destination buffer.
	 * @param frame_length Receives the number of encoded frame bytes.
	 * @return true when the complete frame was encoded.
	 */
	bool uart_frame_encode(
		const uint8_t *payload,
		uint16_t payload_length,
		uint8_t *frame,
		uint16_t frame_capacity,
		uint16_t *frame_length);

	/**
	 * @brief Reset a parser so it can receive a new UART frame.
	 *
	 * @param parser Parser state to reset.
	 */
	void uart_frame_parser_init(uart_frame_parser_t *parser);

	/**
	 * @brief Process one byte from a UART stream.
	 *
	 * @param parser Parser receiving the byte.
	 * @param byte Next byte from the UART stream.
	 * @param frame Receives the payload view when a frame completes.
	 * @return Parser progress, completion, or framing error.
	 */
	uart_frame_result_t uart_frame_parser_process(
		uart_frame_parser_t *parser,
		uint8_t byte,
		uart_frame_view_t *frame);

#ifdef __cplusplus
}
#endif

#endif /* SHARED_UART_FRAMING_H_ */
