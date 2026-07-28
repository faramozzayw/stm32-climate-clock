#include "telemetry.hpp"

#include "device.pb.h"
#include "pb_encode.h"

namespace climate_clock::telemetry::detail
{
bool encode_temperature_to_buffer(
	std::int16_t current_temperature,
	std::int16_t min_temperature,
	std::int16_t max_temperature,
	std::uint8_t *frame,
	std::uint16_t frame_capacity,
	std::uint16_t &frame_length)
{
	device_DeviceMessage message = device_DeviceMessage_init_zero;
	std::uint8_t payload[device_DeviceMessage_size];

	message.which_payload = device_DeviceMessage_telemetry_tag;
	message.payload.telemetry.current_temp = current_temperature;
	message.payload.telemetry.min_temp = min_temperature;
	message.payload.telemetry.max_temp = max_temperature;

	pb_ostream_t stream = pb_ostream_from_buffer(payload, sizeof(payload));
	if (!pb_encode(&stream, device_DeviceMessage_fields, &message))
	{
		return false;
	}

	return uart_frame_encode(
		payload,
		static_cast<std::uint16_t>(stream.bytes_written),
		frame,
		frame_capacity,
		&frame_length);
}
} // namespace climate_clock::telemetry::detail
