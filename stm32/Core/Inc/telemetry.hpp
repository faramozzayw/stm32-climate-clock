#ifndef INC_TELEMETRY_HPP_
#define INC_TELEMETRY_HPP_

#include <cstddef>
#include <cstdint>
#include <limits>

#include "command_receiver/uart_frame.h"

namespace climate_clock::telemetry
{
inline constexpr std::size_t max_frame_size = UART_FRAME_MAX_SIZE;

namespace detail
{
[[nodiscard]] bool encode_temperature_to_buffer(
	std::int16_t current_temperature,
	std::int16_t min_temperature,
	std::int16_t max_temperature,
	std::uint8_t *frame,
	std::uint16_t frame_capacity,
	std::uint16_t &frame_length);
} // namespace detail

template <std::size_t Capacity>
[[nodiscard]] bool encode_temperature(
	std::int16_t current_temperature,
	std::int16_t min_temperature,
	std::int16_t max_temperature,
	std::uint8_t (&frame)[Capacity],
	std::uint16_t &frame_length)
{
	static_assert(
		Capacity <= std::numeric_limits<std::uint16_t>::max());

	return detail::encode_temperature_to_buffer(
		current_temperature,
		min_temperature,
		max_temperature,
		frame,
		static_cast<std::uint16_t>(Capacity),
		frame_length);
}
} // namespace climate_clock::telemetry

#endif /* INC_TELEMETRY_HPP_ */
