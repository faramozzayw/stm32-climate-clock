#include "temperature_settings.h"

#include "byte_codec.h"

/* Each temperature occupies one little-endian int16_t EEPROM slot. */
#define MIN_TEMP_ADDRESS 0U
#define MAX_TEMP_ADDRESS 2U

/**
 * @brief Load one temperature from a specific EEPROM slot.
 *
 * An erased 0xFFFF slot is treated as missing. The output is changed only
 * after a successful read of a populated slot.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param address EEPROM address of the two-byte temperature slot.
 * @param value Temperature output in tenths of a degree Celsius.
 * @return true if a populated slot was read; otherwise false.
 */
static bool load_temperature_setting(const at24c256_t *eeprom, uint16_t address, int16_t *value)
{
	uint8_t encoded[2];

	if (at24c256_read(eeprom, address, encoded, sizeof(encoded)) != AT24C256_OK)
	{
		return false;
	}

	if ((encoded[0] == 0xFFU) && (encoded[1] == 0xFFU))
	{
		return false;
	}

	*value = read_int16_le(encoded);
	return true;
}

/**
 * @brief Save one temperature to a specific EEPROM slot.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param address EEPROM address of the two-byte temperature slot.
 * @param value Temperature in tenths of a degree Celsius.
 * @return AT24C256_OK on success, or an AT24C256 error status.
 */
static AT24C256_Status save_temperature_setting(const at24c256_t *eeprom, uint16_t address, int16_t value)
{
	uint8_t encoded[2];
	write_int16_le(encoded, value);

	return at24c256_write(eeprom, address, encoded, sizeof(encoded));
}

/**
 * @brief Load both temperature settings from their EEPROM slots.
 *
 * Minimum and maximum are loaded independently. A missing or unreadable slot
 * leaves its corresponding output unchanged.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param min_temp Minimum-temperature output in tenths of a degree Celsius.
 * @param max_temp Maximum-temperature output in tenths of a degree Celsius.
 * @return true if at least one setting was loaded; otherwise false.
 */
bool load_temperature_settings(const at24c256_t *eeprom, int16_t *min_temp, int16_t *max_temp)
{
	bool min_loaded;
	bool max_loaded;

	if ((eeprom == NULL) || (min_temp == NULL) || (max_temp == NULL))
	{
		return false;
	}

	min_loaded = load_temperature_setting(eeprom, MIN_TEMP_ADDRESS, min_temp);
	max_loaded = load_temperature_setting(eeprom, MAX_TEMP_ADDRESS, max_temp);

	return min_loaded || max_loaded;
}

/**
 * @brief Save the minimum temperature to the minimum-temperature slot.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param min_temp Minimum temperature in tenths of a degree Celsius.
 * @return AT24C256_OK on success, or an AT24C256 error status.
 */
AT24C256_Status save_min_temperature(const at24c256_t *eeprom, int16_t min_temp)
{
	return save_temperature_setting(eeprom, MIN_TEMP_ADDRESS, min_temp);
}

/**
 * @brief Save the maximum temperature to the maximum-temperature slot.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param max_temp Maximum temperature in tenths of a degree Celsius.
 * @return AT24C256_OK on success, or an AT24C256 error status.
 */
AT24C256_Status save_max_temperature(const at24c256_t *eeprom, int16_t max_temp)
{
	return save_temperature_setting(eeprom, MAX_TEMP_ADDRESS, max_temp);
}
