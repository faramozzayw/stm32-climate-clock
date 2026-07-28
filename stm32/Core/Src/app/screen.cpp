#include "app/screen.hpp"

#include <cstddef>
#include <cstdio>

namespace climate_clock
{
namespace
{
constexpr char time_format[] = "%02u:%02u %02u/%02u/%04u";
constexpr std::size_t time_text_buffer_size =
	sizeof("255:255 255/255/65535");
} // namespace

Screen::Screen(lcd1602_t &lcd)
	: lcd_(lcd)
{
}

void Screen::initialize()
{
	lcd_1602_init(&lcd_);
	lcd_1602_backlight_on(&lcd_);
	lcd_1602_print(&lcd_, "Initializing");
}

void Screen::update(
	const calendar_time_t &time,
	std::int16_t temperature_celsius,
	temperature_unit_t unit)
{
	char time_row[time_text_buffer_size];
	char temperature_row[LCD1602_COLUMN_COUNT + 1U];
	std::int16_t display_temperature = temperature_celsius;
	char temperature_unit = TEMPERATURE_UNIT_CELSIUS_SYMBOL;

	if (unit == TEMPERATURE_UNIT_FAHRENHEIT)
	{
		display_temperature =
			celsius_to_fahrenheit_fixed(temperature_celsius);
		temperature_unit = TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL;
	}

	std::snprintf(time_row,
		sizeof(time_row),
		time_format,
		static_cast<unsigned int>(time.hour),
		static_cast<unsigned int>(time.minute),
		static_cast<unsigned int>(time.day),
		static_cast<unsigned int>(time.month),
		static_cast<unsigned int>(time.year));

	if (fixed_temperature_to_string(
			temperature_row,
			sizeof(temperature_row),
			display_temperature,
			temperature_unit))
	{
		lcd_1602_write_row(&lcd_, 0U, time_row);
		lcd_1602_write_row(&lcd_, 1U, temperature_row);
	}
}
} // namespace climate_clock
