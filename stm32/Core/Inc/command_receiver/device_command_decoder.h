#ifndef INC_DEVICE_COMMAND_DECODER_H_
#define INC_DEVICE_COMMAND_DECODER_H_

#include <stdint.h>

typedef enum
{
	DEVICE_COMMAND_NONE = 0,
	DEVICE_COMMAND_SET_MAX_TEMP,
	DEVICE_COMMAND_SET_MIN_TEMP,
	DEVICE_COMMAND_SET_CURRENT_TIME,
} device_command_type_t;

typedef struct
{
	device_command_type_t type;
	union
	{
		int16_t temperature;
		uint64_t current_time_ms;
	} value;
} decoded_device_command_t;

typedef enum
{
	DEVICE_COMMAND_DECODE_OK = 0,
	DEVICE_COMMAND_DECODE_INVALID_ARGUMENT,
	DEVICE_COMMAND_DECODE_INVALID_PROTOBUF,
	DEVICE_COMMAND_DECODE_MISSING_COMMAND,
	DEVICE_COMMAND_DECODE_VALUE_OUT_OF_RANGE,
} device_command_decode_result_t;

device_command_decode_result_t device_command_decode(
	const uint8_t *payload,
	uint16_t payload_length,
	decoded_device_command_t *command);

#endif /* INC_DEVICE_COMMAND_DECODER_H_ */
