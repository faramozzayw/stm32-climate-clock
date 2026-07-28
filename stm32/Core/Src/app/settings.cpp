#include "app/settings.hpp"

#include "utils/byte_codec.h"

namespace climate_clock
{
namespace
{
/* Temperatures occupy little-endian int16_t slots; the unit occupies one byte. */
constexpr std::uint16_t min_temperature_address = 0U;
constexpr std::uint16_t max_temperature_address = 2U;
constexpr std::uint16_t temperature_unit_address = 4U;
} // namespace

Settings::Settings(const at24c256_t &eeprom)
	: eeprom_(eeprom)
{
}

bool Settings::load(
	std::int16_t &min_temperature,
	std::int16_t &max_temperature,
	temperature_unit_t &unit) const
{
	const bool min_loaded =
		load_temperature(min_temperature_address, min_temperature);
	const bool max_loaded =
		load_temperature(max_temperature_address, max_temperature);
	const bool unit_loaded = load_temperature_unit(unit);

	return min_loaded || max_loaded || unit_loaded;
}

AT24C256_Status Settings::save_min_temperature(
	std::int16_t temperature) const
{
	return save_temperature(min_temperature_address, temperature);
}

AT24C256_Status Settings::save_max_temperature(
	std::int16_t temperature) const
{
	return save_temperature(max_temperature_address, temperature);
}

AT24C256_Status Settings::save_temperature_unit(
	temperature_unit_t unit) const
{
	return at24c256_write_byte(
		&eeprom_,
		temperature_unit_address,
		static_cast<std::uint8_t>(unit));
}

bool Settings::load_temperature(
	std::uint16_t address,
	std::int16_t &temperature) const
{
	std::uint8_t encoded[2];

	if (at24c256_read(
			&eeprom_,
			address,
			encoded,
			sizeof(encoded)) != AT24C256_OK)
	{
		return false;
	}

	if ((encoded[0] == 0xFFU) && (encoded[1] == 0xFFU))
	{
		return false;
	}

	temperature = read_int16_le(encoded);
	return true;
}

bool Settings::load_temperature_unit(
	temperature_unit_t &unit) const
{
	std::uint8_t encoded;

	if (at24c256_read_byte(
			&eeprom_,
			temperature_unit_address,
			&encoded) != AT24C256_OK)
	{
		return false;
	}

	if ((encoded != static_cast<std::uint8_t>(TEMPERATURE_UNIT_CELSIUS)) &&
		(encoded != static_cast<std::uint8_t>(TEMPERATURE_UNIT_FAHRENHEIT)))
	{
		return false;
	}

	unit = static_cast<temperature_unit_t>(encoded);
	return true;
}

AT24C256_Status Settings::save_temperature(
	std::uint16_t address,
	std::int16_t temperature) const
{
	std::uint8_t encoded[2];
	write_int16_le(encoded, temperature);

	return at24c256_write(
		&eeprom_,
		address,
		encoded,
		sizeof(encoded));
}
} // namespace climate_clock
