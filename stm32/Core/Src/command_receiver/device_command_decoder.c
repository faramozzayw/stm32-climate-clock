#include "command_receiver/device_command_decoder.h"

#include <limits.h>
#include <stddef.h>

#include "device.pb.h"
#include "pb_decode.h"

device_command_decode_result_t device_command_decode(
	const uint8_t *payload,
	uint16_t payload_length,
	decoded_device_command_t *command)
{
	device_DeviceCommand protobuf_command = device_DeviceCommand_init_zero;
	pb_istream_t stream;

	if (payload == NULL || payload_length == 0U || command == NULL)
	{
		return DEVICE_COMMAND_DECODE_INVALID_ARGUMENT;
	}

	command->type = DEVICE_COMMAND_NONE;
	command->value.current_time_ms = 0U;
	stream = pb_istream_from_buffer(payload, payload_length);
	if (!pb_decode(&stream, device_DeviceCommand_fields, &protobuf_command) ||
		stream.bytes_left != 0U)
	{
		return DEVICE_COMMAND_DECODE_INVALID_PROTOBUF;
	}

	switch (protobuf_command.which_command)
	{
	case device_DeviceCommand_set_max_temp_tag:
		if (protobuf_command.command.set_max_temp.value < INT16_MIN ||
			protobuf_command.command.set_max_temp.value > INT16_MAX)
		{
			return DEVICE_COMMAND_DECODE_VALUE_OUT_OF_RANGE;
		}
		command->type = DEVICE_COMMAND_SET_MAX_TEMP;
		command->value.temperature =
			(int16_t)protobuf_command.command.set_max_temp.value;
		break;

	case device_DeviceCommand_set_min_temp_tag:
		if (protobuf_command.command.set_min_temp.value < INT16_MIN ||
			protobuf_command.command.set_min_temp.value > INT16_MAX)
		{
			return DEVICE_COMMAND_DECODE_VALUE_OUT_OF_RANGE;
		}
		command->type = DEVICE_COMMAND_SET_MIN_TEMP;
		command->value.temperature =
			(int16_t)protobuf_command.command.set_min_temp.value;
		break;

	case device_DeviceCommand_set_current_time_tag:
		command->type = DEVICE_COMMAND_SET_CURRENT_TIME;
		command->value.current_time_ms =
			protobuf_command.command.set_current_time.value_ms;
		break;

	case 0U:
		return DEVICE_COMMAND_DECODE_MISSING_COMMAND;

	default:
		return DEVICE_COMMAND_DECODE_INVALID_PROTOBUF;
	}

	return DEVICE_COMMAND_DECODE_OK;
}
