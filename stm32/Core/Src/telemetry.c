#include "telemetry.h"

#include <stddef.h>

#include "device.pb.h"
#include "pb_encode.h"

bool telemetry_encode_temperature(
	int16_t current_temperature,
	int16_t min_temperature,
	int16_t max_temperature,
	uint8_t *frame,
	uint16_t frame_capacity,
	uint16_t *frame_length)
{
	device_DeviceMessage message = device_DeviceMessage_init_zero;
	uint8_t payload[device_DeviceMessage_size];
	pb_ostream_t stream;

	if ((frame == NULL) || (frame_length == NULL))
	{
		return false;
	}

	message.which_payload = device_DeviceMessage_telemetry_tag;
	message.payload.telemetry.current_temp = current_temperature;
	message.payload.telemetry.min_temp = min_temperature;
	message.payload.telemetry.max_temp = max_temperature;
	stream = pb_ostream_from_buffer(payload, sizeof(payload));

	if (!pb_encode(&stream, device_DeviceMessage_fields, &message))
	{
		return false;
	}

	return uart_frame_encode(
		payload,
		(uint16_t)stream.bytes_written,
		frame,
		frame_capacity,
		frame_length);
}
