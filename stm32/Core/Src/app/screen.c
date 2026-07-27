#include "screen.h"

#define TIME_FORMAT "%02d:%02d %02d/%02d/%04d"

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
	char temperature_text[16];
	int16_t display_temperature = temperature_celsius;
	char temperature_unit = TEMPERATURE_UNIT_CELSIUS_SYMBOL;

	if (unit == TEMPERATURE_UNIT_FAHRENHEIT)
	{
		display_temperature =
			celsius_to_fahrenheit_fixed(temperature_celsius);
		temperature_unit = TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL;
	}

	lcd_1602_clear(lcd);
	lcd_1602_cur(lcd, 0, 0);
	lcd_1602_printf(lcd,
		TIME_FORMAT,
		time->hour,
		time->minute,
		time->day,
		time->month,
		time->year);

	lcd_1602_cur(lcd, 1, 0);
	if (fixed_temperature_to_string(
			temperature_text,
			sizeof(temperature_text),
			display_temperature,
			temperature_unit))
	{
		lcd_1602_printf(lcd, "%s", temperature_text);
	}
}
