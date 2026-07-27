#include "screen.h"

#include <stdio.h>

#define TIME_FORMAT "%02u:%02u %02u/%02u/%04u"
#define TIME_TEXT_BUFFER_SIZE sizeof("255:255 255/255/65535")

void screen_init(lcd1602_t *lcd)
{
	lcd_1602_init(lcd);
	lcd_1602_backlight_on(lcd);
	lcd_1602_print(lcd, "Initializing");
}

void screen_update(
	lcd1602_t *lcd,
	const calendar_time_t *time,
	int16_t temperature_celsius,
	temperature_unit_t unit)
{
	char time_row[TIME_TEXT_BUFFER_SIZE];
	char temperature_row[LCD1602_COLUMN_COUNT + 1U];
	int16_t display_temperature = temperature_celsius;
	char temperature_unit = TEMPERATURE_UNIT_CELSIUS_SYMBOL;

	if (unit == TEMPERATURE_UNIT_FAHRENHEIT)
	{
		display_temperature =
			celsius_to_fahrenheit_fixed(temperature_celsius);
		temperature_unit = TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL;
	}

	snprintf(time_row,
		sizeof(time_row),
		TIME_FORMAT,
		(unsigned int)time->hour,
		(unsigned int)time->minute,
		(unsigned int)time->day,
		(unsigned int)time->month,
		(unsigned int)time->year);

	if (fixed_temperature_to_string(
			temperature_row,
			sizeof(temperature_row),
			display_temperature,
			temperature_unit))
	{
		lcd_1602_write_row(lcd, 0U, time_row);
		lcd_1602_write_row(lcd, 1U, temperature_row);
	}
}
