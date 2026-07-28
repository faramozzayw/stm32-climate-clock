#include "telemetry.hpp"

#include "device.pb.h"
#include "pb_encode.h"

namespace climate_clock::telemetry
{
namespace
{
bool encode_telemetry(
	const device_DeviceTelemetry &telemetry,
	std::uint8_t *frame,
	std::uint16_t frame_capacity,
	std::uint16_t &frame_length)
{
	device_DeviceMessage message = device_DeviceMessage_init_zero;
	std::uint8_t payload[device_DeviceMessage_size];

	message.which_payload = device_DeviceMessage_telemetry_tag;
	message.payload.telemetry = telemetry;

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
} // namespace

namespace detail
{
bool encode_measurement_to_buffer(
	std::int16_t temperature_tenths_celsius,
	const std::optional<std::uint32_t> &humidity_milli_percent,
	std::uint8_t *frame,
	std::uint16_t frame_capacity,
	std::uint16_t &frame_length)
{
	device_DeviceTelemetry telemetry = device_DeviceTelemetry_init_zero;
	telemetry.which_data = device_DeviceTelemetry_measurement_tag;
	telemetry.data.measurement.temperature_tenths_celsius =
		temperature_tenths_celsius;

	if (humidity_milli_percent.has_value())
	{
		telemetry.data.measurement.has_humidity_milli_percent = true;
		telemetry.data.measurement.humidity_milli_percent =
			humidity_milli_percent.value();
	}

	return encode_telemetry(
		telemetry,
		frame,
		frame_capacity,
		frame_length);
}

bool encode_settings_to_buffer(
	std::int16_t min_temperature,
	std::int16_t max_temperature,
	std::uint16_t min_humidity_tenths_percent,
	std::uint16_t max_humidity_tenths_percent,
	temperature_unit_t temperature_unit,
	std::uint8_t *frame,
	std::uint16_t frame_capacity,
	std::uint16_t &frame_length)
{
	device_DeviceTelemetry telemetry = device_DeviceTelemetry_init_zero;
	telemetry.which_data = device_DeviceTelemetry_settings_tag;
	telemetry.data.settings.min_temperature_tenths_celsius =
		min_temperature;
	telemetry.data.settings.max_temperature_tenths_celsius =
		max_temperature;
	telemetry.data.settings.min_humidity_tenths_percent =
		min_humidity_tenths_percent;
	telemetry.data.settings.max_humidity_tenths_percent =
		max_humidity_tenths_percent;
	telemetry.data.settings.temperature_unit =
		temperature_unit == TEMPERATURE_UNIT_FAHRENHEIT
			? device_TemperatureUnit_TEMPERATURE_UNIT_FAHRENHEIT
			: device_TemperatureUnit_TEMPERATURE_UNIT_CELSIUS;

	return encode_telemetry(
		telemetry,
		frame,
		frame_capacity,
		frame_length);
}
} // namespace detail
} // namespace climate_clock::telemetry
