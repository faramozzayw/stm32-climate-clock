#ifndef INC_TELEMETRY_H_
#define INC_TELEMETRY_H_

#include <stdbool.h>
#include <stdint.h>

#include "command_receiver/uart_frame.h"

#define TELEMETRY_FRAME_MAX_SIZE UART_FRAME_MAX_SIZE

/**
 * @brief Encode temperature values as a framed DeviceTelemetry message.
 *
 * @param current_temperature Current temperature in tenths of a degree Celsius.
 * @param min_temperature Configured minimum in tenths of a degree Celsius.
 * @param max_temperature Configured maximum in tenths of a degree Celsius.
 * @param frame Destination for the encoded UART frame.
 * @param frame_capacity Capacity of the destination buffer.
 * @param frame_length Receives the number of encoded frame bytes.
 * @return true when the complete frame was encoded.
 */
bool telemetry_encode_temperature(
	int16_t current_temperature,
	int16_t min_temperature,
	int16_t max_temperature,
	uint8_t *frame,
	uint16_t frame_capacity,
	uint16_t *frame_length);

#endif /* INC_TELEMETRY_H_ */
