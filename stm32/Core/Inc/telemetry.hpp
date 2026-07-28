#ifndef INC_TELEMETRY_HPP_
#define INC_TELEMETRY_HPP_

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>

#include "command_receiver/uart_frame.h"

namespace climate_clock::telemetry
{
inline constexpr std::size_t max_frame_size = UART_FRAME_MAX_SIZE;

namespace detail
{
[[nodiscard]] bool encode_measurement_to_buffer(
	std::int16_t temperature_tenths_celsius,
	const std::optional<std::uint32_t> &humidity_milli_percent,
	std::uint8_t *frame,
	std::uint16_t frame_capacity,
	std::uint16_t &frame_length);

[[nodiscard]] bool encode_limits_to_buffer(
	std::int16_t min_temperature,
	std::int16_t max_temperature,
	std::uint16_t min_humidity_tenths_percent,
	std::uint16_t max_humidity_tenths_percent,
	std::uint8_t *frame,
	std::uint16_t frame_capacity,
	std::uint16_t &frame_length);
} // namespace detail

template <std::size_t Capacity>
[[nodiscard]] bool encode_measurement(
	std::int16_t temperature_tenths_celsius,
	const std::optional<std::uint32_t> &humidity_milli_percent,
	std::uint8_t (&frame)[Capacity],
	std::uint16_t &frame_length)
{
	static_assert(
		Capacity <= std::numeric_limits<std::uint16_t>::max());

	return detail::encode_measurement_to_buffer(
		temperature_tenths_celsius,
		humidity_milli_percent,
		frame,
		static_cast<std::uint16_t>(Capacity),
		frame_length);
}

template <std::size_t Capacity>
[[nodiscard]] bool encode_limits(
	std::int16_t min_temperature,
	std::int16_t max_temperature,
	std::uint16_t min_humidity_tenths_percent,
	std::uint16_t max_humidity_tenths_percent,
	std::uint8_t (&frame)[Capacity],
	std::uint16_t &frame_length)
{
	static_assert(
		Capacity <= std::numeric_limits<std::uint16_t>::max());

	return detail::encode_limits_to_buffer(
		min_temperature,
		max_temperature,
		min_humidity_tenths_percent,
		max_humidity_tenths_percent,
		frame,
		static_cast<std::uint16_t>(Capacity),
		frame_length);
}
} // namespace climate_clock::telemetry

#endif /* INC_TELEMETRY_HPP_ */
