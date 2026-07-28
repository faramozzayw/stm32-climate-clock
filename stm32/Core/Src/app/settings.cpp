#include "app/settings.hpp"

#include "utils/byte_codec.h"

namespace climate_clock
{
namespace
{
/* Numeric limits occupy little-endian int16_t slots; the unit occupies one byte. */
constexpr std::uint16_t min_temperature_address = 0U;
constexpr std::uint16_t max_temperature_address = 2U;
constexpr std::uint16_t temperature_unit_address = 4U;
constexpr std::uint16_t min_humidity_address = 5U;
constexpr std::uint16_t max_humidity_address = 7U;
constexpr std::uint16_t maximum_humidity_tenths_percent = 1000U;
} // namespace

Settings::Settings(const at24c256_t &eeprom)
	: eeprom_(eeprom)
{
}

bool Settings::load(
	std::int16_t &min_temperature,
	std::int16_t &max_temperature,
	std::uint16_t &min_humidity_tenths_percent,
	std::uint16_t &max_humidity_tenths_percent,
	temperature_unit_t &unit) const
{
	const bool min_loaded =
		load_int16(min_temperature_address, min_temperature);
	const bool max_loaded =
		load_int16(max_temperature_address, max_temperature);
	const bool unit_loaded = load_temperature_unit(unit);
	const bool min_humidity_loaded = load_humidity(
		min_humidity_address,
		min_humidity_tenths_percent);
	const bool max_humidity_loaded = load_humidity(
		max_humidity_address,
		max_humidity_tenths_percent);

	return min_loaded ||
		   max_loaded ||
		   unit_loaded ||
		   min_humidity_loaded ||
		   max_humidity_loaded;
}

AT24C256_Status Settings::save_min_temperature(
	std::int16_t temperature) const
{
	return save_int16(min_temperature_address, temperature);
}

AT24C256_Status Settings::save_max_temperature(
	std::int16_t temperature) const
{
	return save_int16(max_temperature_address, temperature);
}

AT24C256_Status Settings::save_min_humidity(
	std::uint16_t humidity_tenths_percent) const
{
	return save_int16(
		min_humidity_address,
		static_cast<std::int16_t>(humidity_tenths_percent));
}

AT24C256_Status Settings::save_max_humidity(
	std::uint16_t humidity_tenths_percent) const
{
	return save_int16(
		max_humidity_address,
		static_cast<std::int16_t>(humidity_tenths_percent));
}

AT24C256_Status Settings::save_temperature_unit(
	temperature_unit_t unit) const
{
	return at24c256_write_byte(
		&eeprom_,
		temperature_unit_address,
		static_cast<std::uint8_t>(unit));
}

bool Settings::load_int16(
	std::uint16_t address,
	std::int16_t &value) const
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

	value = read_int16_le(encoded);
	return true;
}

bool Settings::load_humidity(
	std::uint16_t address,
	std::uint16_t &humidity_tenths_percent) const
{
	std::int16_t stored_humidity;
	if (!load_int16(address, stored_humidity) ||
		(stored_humidity < 0) ||
		(stored_humidity >
			static_cast<std::int16_t>(
				maximum_humidity_tenths_percent)))
	{
		return false;
	}

	humidity_tenths_percent =
		static_cast<std::uint16_t>(stored_humidity);
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

AT24C256_Status Settings::save_int16(
	std::uint16_t address,
	std::int16_t value) const
{
	std::uint8_t encoded[2];
	write_int16_le(encoded, value);

	return at24c256_write(
		&eeprom_,
		address,
		encoded,
		sizeof(encoded));
}
} // namespace climate_clock
