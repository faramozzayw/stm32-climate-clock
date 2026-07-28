#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern "C"
{
#include "unity.h"
}

#include "app/screen.hpp"

using climate_clock::Screen;

static uint32_t init_call_count;
static uint32_t backlight_call_count;
static uint32_t print_call_count;
static uint32_t write_row_call_count;
static char requested_rows[2][17];

void lcd_1602_init(lcd1602_t *lcd)
{
	(void)lcd;
	init_call_count++;
}

void lcd_1602_backlight_on(lcd1602_t *lcd)
{
	(void)lcd;
	backlight_call_count++;
}

void lcd_1602_print(lcd1602_t *lcd, const char *text)
{
	(void)lcd;
	(void)text;
	print_call_count++;
}

void lcd_1602_write_row(lcd1602_t *lcd, uint8_t row, const char *text)
{
	(void)lcd;
	write_row_call_count++;
	snprintf(requested_rows[row], sizeof(requested_rows[row]), "%s", text);
}

static void reset_mocks(void)
{
	init_call_count = 0U;
	backlight_call_count = 0U;
	print_call_count = 0U;
	write_row_call_count = 0U;
	memset(requested_rows, 0, sizeof(requested_rows));
}

static calendar_time_t sample_time(void)
{
	calendar_time_t time{};
	time.second = 56U;
	time.minute = 34U;
	time.hour = 12U;
	time.weekday = 2U;
	time.day = 27U;
	time.month = 7U;
	time.year = 2026U;

	return time;
}

static void test_screen_initializes_and_writes_fixed_width_rows(void)
{
	lcd1602_t lcd;
	calendar_time_t time = sample_time();
	Screen screen{lcd};

	reset_mocks();
	screen.initialize();
	TEST_ASSERT_TRUE(init_call_count == 1U);
	TEST_ASSERT_TRUE(backlight_call_count == 1U);
	TEST_ASSERT_TRUE(print_call_count == 1U);

	screen.update(time, 253, TEMPERATURE_UNIT_CELSIUS);
	TEST_ASSERT_TRUE(write_row_call_count == 2U);
	TEST_ASSERT_TRUE(strcmp(requested_rows[0], "12:34 27/07/2026") == 0);
	TEST_ASSERT_TRUE(strcmp(requested_rows[1], "25.3 C") == 0);
}

static void test_screen_formats_fahrenheit(void)
{
	lcd1602_t lcd;
	calendar_time_t time = sample_time();
	Screen screen{lcd};

	reset_mocks();
	screen.initialize();
	screen.update(time, 100, TEMPERATURE_UNIT_FAHRENHEIT);
	TEST_ASSERT_TRUE(strcmp(requested_rows[1], "50.0 F") == 0);
}

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_screen_initializes_and_writes_fixed_width_rows);
	RUN_TEST(test_screen_formats_fahrenheit);
	return UNITY_END();
}
