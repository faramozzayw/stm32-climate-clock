#ifndef INC_TEMPERATURE_SETTINGS_H_
#define INC_TEMPERATURE_SETTINGS_H_

#include <stdbool.h>
#include <stdint.h>

#include "drivers/at24c256.h"

/**
 * @brief Load the minimum and maximum temperatures from EEPROM.
 *
 * Each setting is loaded independently. If a setting is missing or cannot be
 * read, its output value is left unchanged so the caller's default remains in
 * effect.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param min_temp Minimum-temperature output, initialized to its fallback.
 * @param max_temp Maximum-temperature output, initialized to its fallback.
 * @return true if at least one setting was loaded; otherwise false.
 */
bool load_temperature_settings(const at24c256_t *eeprom,
                               int16_t *min_temp,
                               int16_t *max_temp);

/**
 * @brief Save the minimum temperature to its EEPROM slot.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param min_temp Minimum temperature in tenths of a degree Celsius.
 * @return AT24C256_OK on success, or an AT24C256 error status.
 */
AT24C256_Status save_min_temperature(const at24c256_t *eeprom,
                                     int16_t min_temp);

/**
 * @brief Save the maximum temperature to its EEPROM slot.
 *
 * @param eeprom Initialized AT24C256 device.
 * @param max_temp Maximum temperature in tenths of a degree Celsius.
 * @return AT24C256_OK on success, or an AT24C256 error status.
 */
AT24C256_Status save_max_temperature(const at24c256_t *eeprom,
                                     int16_t max_temp);

#endif /* INC_TEMPERATURE_SETTINGS_H_ */
