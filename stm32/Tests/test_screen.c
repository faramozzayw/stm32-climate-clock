#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "app/screen.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define CHECK(condition)                                                       \
	do                                                                         \
	{                                                                          \
		if (!(condition))                                                      \
		{                                                                      \
			printf("    CHECK failed at line %d: %s\n", __LINE__, #condition); \
			return false;                                                      \
		}                                                                      \
	} while (0)

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
	calendar_time_t time = {
		.second = 56U,
		.minute = 34U,
		.hour = 12U,
		.weekday = 2U,
		.day = 27U,
		.month = 7U,
		.year = 2026U,
	};

	return time;
}

static bool test_screen_initializes_and_writes_fixed_width_rows(void)
{
	lcd1602_t lcd;
	calendar_time_t time = sample_time();

	reset_mocks();
	screen_init(&lcd);
	CHECK(init_call_count == 1U);
	CHECK(backlight_call_count == 1U);
	CHECK(print_call_count == 1U);

	screen_update(&lcd, &time, 253, TEMPERATURE_UNIT_CELSIUS);
	CHECK(write_row_call_count == 2U);
	CHECK(strcmp(requested_rows[0], "12:34 27/07/2026") == 0);
	CHECK(strcmp(requested_rows[1], "25.3 C") == 0);
	return true;
}

static bool test_screen_formats_fahrenheit(void)
{
	lcd1602_t lcd;
	calendar_time_t time = sample_time();

	reset_mocks();
	screen_init(&lcd);
	screen_update(&lcd, &time, 100, TEMPERATURE_UNIT_FAHRENHEIT);
	CHECK(strcmp(requested_rows[1], "50.0 F") == 0);
	return true;
}

typedef bool (*test_function_t)(void);

typedef struct
{
	const char *name;
	test_function_t function;
} test_case_t;

int main(void)
{
	const test_case_t tests[] = {
		{"screen formats rows", test_screen_initializes_and_writes_fixed_width_rows},
		{"screen formats Fahrenheit", test_screen_formats_fahrenheit},
	};
	size_t pos;
	size_t passed = 0U;

	for (pos = 0U; pos < ARRAY_SIZE(tests); pos++)
	{
		printf("[ RUN      ] %s\n", tests[pos].name);
		if (tests[pos].function())
		{
			printf("[       OK ] %s\n", tests[pos].name);
			passed++;
		}
		else
		{
			printf("[  FAILED  ] %s\n", tests[pos].name);
		}
	}

	printf("\n%zu/%zu tests passed\n", passed, ARRAY_SIZE(tests));
	return passed == ARRAY_SIZE(tests) ? 0 : 1;
}
