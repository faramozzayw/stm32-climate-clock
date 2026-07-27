#ifndef SRC_APP_SCREEN_H_
#define SRC_APP_SCREEN_H_

#include <stdint.h>

#include "utils/calendar_time.h"
#include "drivers/lcd1602.h"
#include "utils/temperature.h"

/**
 * @brief Initialize the LCD and show the startup message.
 *
 * @param lcd Configured LCD device.
 */
void screen_init(lcd1602_t *lcd);

/**
 * @brief Render the current time, date, and temperature on the LCD.
 *
 * @param lcd Initialized LCD device.
 * @param time Current RTC time and date.
 * @param temperature_celsius Temperature in tenths of a degree Celsius.
 * @param unit Unit in which the temperature should be displayed.
 */
void screen_update(
	lcd1602_t *lcd,
	const calendar_time_t *time,
	int16_t temperature_celsius,
	temperature_unit_t unit);

#endif /* SRC_APP_SCREEN_H_ */
