#include "command_receiver/device_message_decoder.h"

#include <limits.h>
#include <stddef.h>

#include "device.pb.h"
#include "pb_decode.h"

static device_message_decode_result_t decode_command(
	const device_DeviceCommand *command,
	decoded_device_message_t *message);

device_message_decode_result_t device_message_decode(
	const uint8_t *payload,
	uint16_t payload_length,
	decoded_device_message_t *message)
{
	device_DeviceMessage protobuf_message = device_DeviceMessage_init_zero;
	pb_istream_t stream;

	if (payload == NULL || payload_length == 0U || message == NULL)
	{
		return DEVICE_MESSAGE_DECODE_INVALID_ARGUMENT;
	}

	message->type = DEVICE_MESSAGE_NONE;
	message->value.current_time_ms = 0U;
	stream = pb_istream_from_buffer(payload, payload_length);
	if (!pb_decode(&stream, device_DeviceMessage_fields, &protobuf_message) ||
		stream.bytes_left != 0U)
	{
		return DEVICE_MESSAGE_DECODE_INVALID_PROTOBUF;
	}

	switch (protobuf_message.which_payload)
	{
	case device_DeviceMessage_command_tag:
		return decode_command(&protobuf_message.payload.command, message);

	case device_DeviceMessage_bridge_status_tag:
		message->type = DEVICE_MESSAGE_BLE_CONNECTION_STATE;

		switch (protobuf_message.payload.bridge_status.ble_connection_state)
		{
		case device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTED:
			message->value.ble_connection_state =
				BLE_CONNECTION_STATE_DISCONNECTED;
			break;

		case device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTING:
			message->value.ble_connection_state =
				BLE_CONNECTION_STATE_CONNECTING;
			break;

		case device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTED:
			message->value.ble_connection_state =
				BLE_CONNECTION_STATE_CONNECTED;
			break;

		case device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTING:
			message->value.ble_connection_state =
				BLE_CONNECTION_STATE_DISCONNECTING;
			break;

		default:
			return DEVICE_MESSAGE_DECODE_VALUE_OUT_OF_RANGE;
		}

		return DEVICE_MESSAGE_DECODE_OK;

	case device_DeviceMessage_telemetry_tag:
		return DEVICE_MESSAGE_DECODE_UNEXPECTED_PAYLOAD;

	case 0U:
		return DEVICE_MESSAGE_DECODE_MISSING_PAYLOAD;

	default:
		return DEVICE_MESSAGE_DECODE_INVALID_PROTOBUF;
	}
}

static device_message_decode_result_t decode_command(
	const device_DeviceCommand *command,
	decoded_device_message_t *message)
{
	switch (command->which_command)
	{
	case device_DeviceCommand_set_max_temp_tag:
		if (command->command.set_max_temp.value < INT16_MIN ||
			command->command.set_max_temp.value > INT16_MAX)
		{
			return DEVICE_MESSAGE_DECODE_VALUE_OUT_OF_RANGE;
		}
		message->type = DEVICE_MESSAGE_SET_MAX_TEMP;
		message->value.temperature =
			(int16_t)command->command.set_max_temp.value;
		break;

	case device_DeviceCommand_set_min_temp_tag:
		if (command->command.set_min_temp.value < INT16_MIN ||
			command->command.set_min_temp.value > INT16_MAX)
		{
			return DEVICE_MESSAGE_DECODE_VALUE_OUT_OF_RANGE;
		}
		message->type = DEVICE_MESSAGE_SET_MIN_TEMP;
		message->value.temperature =
			(int16_t)command->command.set_min_temp.value;
		break;

	case device_DeviceCommand_set_current_time_tag:
		message->type = DEVICE_MESSAGE_SET_CURRENT_TIME;
		message->value.current_time_ms =
			command->command.set_current_time.value_ms;
		break;

	case 0U:
		return DEVICE_MESSAGE_DECODE_MISSING_COMMAND;

	default:
		return DEVICE_MESSAGE_DECODE_INVALID_PROTOBUF;
	}

	return DEVICE_MESSAGE_DECODE_OK;
}
