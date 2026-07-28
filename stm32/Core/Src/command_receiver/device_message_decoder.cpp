#include "command_receiver/device_message_decoder.hpp"

#include <climits>
#include <utility>

#include "device.pb.h"
#include "pb_decode.h"

namespace climate_clock
{
namespace
{
constexpr std::uint32_t maximum_humidity_tenths_percent = 1000U;

DecodeResult decode_command(const device_DeviceCommand &command)
{
	switch (command.which_command)
	{
	case device_DeviceCommand_set_max_temp_tag:
		if ((command.command.set_max_temp.value < INT16_MIN) ||
			(command.command.set_max_temp.value > INT16_MAX))
		{
			return DecodeResult::failure(DecodeError::value_out_of_range);
		}

		return DecodeResult::success(DecodedDeviceMessage{
			SetMaximumTemperature{
				static_cast<std::int16_t>(
					command.command.set_max_temp.value)}});

	case device_DeviceCommand_set_min_temp_tag:
		if ((command.command.set_min_temp.value < INT16_MIN) ||
			(command.command.set_min_temp.value > INT16_MAX))
		{
			return DecodeResult::failure(DecodeError::value_out_of_range);
		}

		return DecodeResult::success(DecodedDeviceMessage{
			SetMinimumTemperature{
				static_cast<std::int16_t>(
					command.command.set_min_temp.value)}});

	case device_DeviceCommand_set_max_humidity_tag:
		if (command.command.set_max_humidity.value >
			maximum_humidity_tenths_percent)
		{
			return DecodeResult::failure(DecodeError::value_out_of_range);
		}

		return DecodeResult::success(DecodedDeviceMessage{
			SetMaximumHumidity{
				static_cast<std::uint16_t>(
					command.command.set_max_humidity.value)}});

	case device_DeviceCommand_set_min_humidity_tag:
		if (command.command.set_min_humidity.value >
			maximum_humidity_tenths_percent)
		{
			return DecodeResult::failure(DecodeError::value_out_of_range);
		}

		return DecodeResult::success(DecodedDeviceMessage{
			SetMinimumHumidity{
				static_cast<std::uint16_t>(
					command.command.set_min_humidity.value)}});

	case device_DeviceCommand_set_current_time_tag:
		return DecodeResult::success(DecodedDeviceMessage{
			SetCurrentTime{
				command.command.set_current_time.value_ms}});

	case device_DeviceCommand_set_temperature_unit_tag:
		switch (command.command.set_temperature_unit.unit)
		{
		case device_TemperatureUnit_TEMPERATURE_UNIT_CELSIUS:
			return DecodeResult::success(DecodedDeviceMessage{
				SetTemperatureUnit{
					DEVICE_TEMPERATURE_UNIT_CELSIUS}});

		case device_TemperatureUnit_TEMPERATURE_UNIT_FAHRENHEIT:
			return DecodeResult::success(DecodedDeviceMessage{
				SetTemperatureUnit{
					DEVICE_TEMPERATURE_UNIT_FAHRENHEIT}});

		default:
			return DecodeResult::failure(
				DecodeError::value_out_of_range);
		}

	case 0U:
		return DecodeResult::failure(DecodeError::missing_command);

	default:
		return DecodeResult::failure(DecodeError::invalid_protobuf);
	}
}

DecodeResult decode_connection_state(
	device_BleConnectionState protobuf_state)
{
	switch (protobuf_state)
	{
	case device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTED:
		return DecodeResult::success(DecodedDeviceMessage{
			BleConnectionStateChanged{
				BLE_CONNECTION_STATE_DISCONNECTED}});

	case device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTING:
		return DecodeResult::success(DecodedDeviceMessage{
			BleConnectionStateChanged{
				BLE_CONNECTION_STATE_CONNECTING}});

	case device_BleConnectionState_BLE_CONNECTION_STATE_CONNECTED:
		return DecodeResult::success(DecodedDeviceMessage{
			BleConnectionStateChanged{
				BLE_CONNECTION_STATE_CONNECTED}});

	case device_BleConnectionState_BLE_CONNECTION_STATE_DISCONNECTING:
		return DecodeResult::success(DecodedDeviceMessage{
			BleConnectionStateChanged{
				BLE_CONNECTION_STATE_DISCONNECTING}});

	default:
		return DecodeResult::failure(DecodeError::value_out_of_range);
	}
}
} // namespace

DecodeResult::DecodeResult(
	std::optional<DecodedDeviceMessage> message,
	DecodeError error)
	: message_(std::move(message)),
	  error_(error)
{
}

DecodeResult DecodeResult::success(DecodedDeviceMessage message)
{
	return DecodeResult{
		std::move(message),
		DecodeError::none};
}

DecodeResult DecodeResult::failure(DecodeError error)
{
	return DecodeResult{std::nullopt, error};
}

DecodeResult::operator bool() const noexcept
{
	return message_.has_value();
}

const DecodedDeviceMessage &DecodeResult::value() const noexcept
{
	return *message_;
}

DecodeError DecodeResult::error() const noexcept
{
	return error_;
}

DecodeResult decode_device_message(
	const std::uint8_t *payload,
	std::size_t payload_length)
{
	if ((payload == nullptr) || (payload_length == 0U))
	{
		return DecodeResult::failure(DecodeError::invalid_argument);
	}

	device_DeviceMessage protobuf_message =
		device_DeviceMessage_init_zero;
	auto stream = pb_istream_from_buffer(payload, payload_length);

	if (!pb_decode(
			&stream,
			device_DeviceMessage_fields,
			&protobuf_message) ||
		(stream.bytes_left != 0U))
	{
		return DecodeResult::failure(DecodeError::invalid_protobuf);
	}

	switch (protobuf_message.which_payload)
	{
	case device_DeviceMessage_command_tag:
		return decode_command(protobuf_message.payload.command);

	case device_DeviceMessage_bridge_status_tag:
		return decode_connection_state(
			protobuf_message.payload.bridge_status.ble_connection_state);

	case device_DeviceMessage_telemetry_tag:
		return DecodeResult::failure(DecodeError::unexpected_payload);

	case 0U:
		return DecodeResult::failure(DecodeError::missing_payload);

	default:
		return DecodeResult::failure(DecodeError::invalid_protobuf);
	}
}
} // namespace climate_clock
