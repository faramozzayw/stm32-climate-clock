#ifndef INC_COMMAND_RECEIVER_DEVICE_MESSAGE_DECODER_HPP_
#define INC_COMMAND_RECEIVER_DEVICE_MESSAGE_DECODER_HPP_

#include <cstddef>
#include <cstdint>
#include <optional>
#include <variant>

#include "command_receiver/device_types.h"

namespace climate_clock
{
struct SetMaximumTemperature
{
	std::int16_t temperature;
};

struct SetMinimumTemperature
{
	std::int16_t temperature;
};

struct SetMaximumHumidity
{
	std::uint16_t humidity_tenths_percent;
};

struct SetMinimumHumidity
{
	std::uint16_t humidity_tenths_percent;
};

struct SetCurrentTime
{
	std::uint64_t time_ms;
};

struct SetTemperatureUnit
{
	device_temperature_unit_t unit;
};

struct BleConnectionStateChanged
{
	ble_connection_state_t state;
};

using DecodedDeviceMessage = std::variant<
	SetMaximumTemperature,
	SetMinimumTemperature,
	SetMaximumHumidity,
	SetMinimumHumidity,
	SetCurrentTime,
	SetTemperatureUnit,
	BleConnectionStateChanged>;

enum class DecodeError : std::uint8_t
{
	none = 0,
	invalid_argument,
	invalid_protobuf,
	missing_payload,
	missing_command,
	unexpected_payload,
	value_out_of_range,
};

class DecodeResult
{
  public:
	[[nodiscard]] static DecodeResult success(
		DecodedDeviceMessage message);
	[[nodiscard]] static DecodeResult failure(DecodeError error);

	[[nodiscard]] explicit operator bool() const noexcept;
	[[nodiscard]] const DecodedDeviceMessage &value() const noexcept;
	[[nodiscard]] DecodeError error() const noexcept;

  private:
	DecodeResult(
		std::optional<DecodedDeviceMessage> message,
		DecodeError error);

	std::optional<DecodedDeviceMessage> message_;
	DecodeError error_;
};

[[nodiscard]] DecodeResult decode_device_message(
	const std::uint8_t *payload,
	std::size_t payload_length);
} // namespace climate_clock

#endif /* INC_COMMAND_RECEIVER_DEVICE_MESSAGE_DECODER_HPP_ */
