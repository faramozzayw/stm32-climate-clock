#ifndef SRC_APP_TEMPERATURE_INDICATOR_H_
#define SRC_APP_TEMPERATURE_INDICATOR_H_

#include <stdint.h>

#include "drivers/hw479.h"

/**
 * @brief Initialize the RGB temperature indicator.
 *
 * @param indicator Configured RGB indicator device.
 */
void temperature_indicator_init(hw479_t *indicator);

/**
 * @brief Update the indicator color from the current temperature thresholds.
 *
 * Blue indicates a temperature at or below the minimum, red indicates a
 * temperature at or above the maximum, and off indicates the normal range.
 * All temperature values use tenths of a degree Celsius.
 *
 * @param indicator Initialized RGB indicator device.
 * @param temperature Current temperature.
 * @param min_temperature Minimum configured temperature.
 * @param max_temperature Maximum configured temperature.
 */
void temperature_indicator_update(hw479_t *indicator, int16_t temperature, int16_t min_temperature, int16_t max_temperature);

#endif /* SRC_APP_TEMPERATURE_INDICATOR_H_ */
