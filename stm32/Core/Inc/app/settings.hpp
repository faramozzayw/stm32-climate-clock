#ifndef INC_APP_SETTINGS_HPP_
#define INC_APP_SETTINGS_HPP_

#include <cstdint>

#include "drivers/at24c256.h"
#include "utils/temperature.h"

namespace climate_clock
{
class Settings
{
  public:
	explicit Settings(const at24c256_t &eeprom);

	/**
	 * Load each available setting while preserving the supplied defaults for
	 * missing or unreadable values.
	 */
	[[nodiscard]] bool load(
		std::int16_t &min_temperature,
		std::int16_t &max_temperature,
		temperature_unit_t &unit) const;

	[[nodiscard]] AT24C256_Status save_min_temperature(
		std::int16_t temperature) const;
	[[nodiscard]] AT24C256_Status save_max_temperature(
		std::int16_t temperature) const;
	[[nodiscard]] AT24C256_Status save_temperature_unit(
		temperature_unit_t unit) const;

  private:
	[[nodiscard]] bool load_temperature(
		std::uint16_t address,
		std::int16_t &temperature) const;
	[[nodiscard]] bool load_temperature_unit(
		temperature_unit_t &unit) const;
	[[nodiscard]] AT24C256_Status save_temperature(
		std::uint16_t address,
		std::int16_t temperature) const;

	const at24c256_t &eeprom_;
};
} // namespace climate_clock

#endif /* INC_APP_SETTINGS_HPP_ */
