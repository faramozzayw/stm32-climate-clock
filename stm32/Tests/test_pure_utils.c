#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "byte_codec.h"
#include "calendar_time.h"
#include "temperature.h"

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

static bool test_temperature_converts_to_fixed_point(void)
{
	CHECK(tempToFixed(12.34f) == 123);
	CHECK(tempToFixed(12.35f) == 124);
	CHECK(tempToFixed(-0.05f) == -1);
	return true;
}

static bool test_temperature_converts_between_units(void)
{
	CHECK(celsius_to_fahrenheit_fixed(0) == 320);
	CHECK(celsius_to_fahrenheit_fixed(100) == 500);
	CHECK(celsius_to_fahrenheit_fixed(-400) == -400);
	return true;
}

static bool test_temperature_formats_signed_values(void)
{
	char buffer[16];

	CHECK(fixed_temperature_to_string(
		buffer, sizeof(buffer), 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	CHECK(strcmp(buffer, "12.3 C") == 0);

	CHECK(fixed_temperature_to_string(
		buffer, sizeof(buffer), -5, TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL));
	CHECK(strcmp(buffer, "-0.5 F") == 0);

	CHECK(fixed_temperature_to_string(
		buffer, sizeof(buffer), INT16_MIN, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	CHECK(strcmp(buffer, "-3276.8 C") == 0);
	return true;
}

static bool test_temperature_format_rejects_invalid_destination(void)
{
	char buffer[5];

	CHECK(!fixed_temperature_to_string(
		NULL, sizeof(buffer), 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	CHECK(!fixed_temperature_to_string(
		buffer, 0U, 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	CHECK(!fixed_temperature_to_string(
		buffer, sizeof(buffer), 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	return true;
}

static bool test_byte_codec_round_trips_signed_values(void)
{
	const int16_t values[] = {0, 1, -1, INT16_MAX, INT16_MIN, 0x1234};
	size_t pos;

	for (pos = 0U; pos < ARRAY_SIZE(values); pos++)
	{
		uint8_t encoded[2];

		write_int16_le(encoded, values[pos]);
		CHECK(read_int16_le(encoded) == values[pos]);
	}

	return true;
}

static bool test_byte_codec_uses_little_endian_order(void)
{
	uint8_t encoded[2];

	write_int16_le(encoded, 0x1234);
	CHECK(encoded[0] == 0x34U);
	CHECK(encoded[1] == 0x12U);
	return true;
}

static bool test_calendar_time_converts_unix_epoch(void)
{
	calendar_time_t time;

	CHECK(calendar_time_from_unix_ms(946684800000ULL, &time));
	CHECK(time.year == 2000U);
	CHECK(time.month == 1U);
	CHECK(time.day == 1U);
	CHECK(time.weekday == 7U);
	CHECK(time.hour == 0U);
	CHECK(time.minute == 0U);
	CHECK(time.second == 0U);
	return true;
}

static bool test_calendar_time_handles_leap_day_and_milliseconds(void)
{
	calendar_time_t time;

	CHECK(calendar_time_from_unix_ms(951827696789ULL, &time));
	CHECK(time.year == 2000U);
	CHECK(time.month == 2U);
	CHECK(time.day == 29U);
	CHECK(time.weekday == 3U);
	CHECK(time.hour == 12U);
	CHECK(time.minute == 34U);
	CHECK(time.second == 56U);
	return true;
}

static bool test_calendar_time_rejects_unsupported_range(void)
{
	calendar_time_t time = {.year = 1234U};

	CHECK(!calendar_time_from_unix_ms(946684799999ULL, &time));
	CHECK(time.year == 1234U);
	CHECK(!calendar_time_from_unix_ms(4102444800000ULL, &time));
	CHECK(time.year == 1234U);
	CHECK(!calendar_time_from_unix_ms(946684800000ULL, NULL));
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
		{"temperature converts to fixed point", test_temperature_converts_to_fixed_point},
		{"temperature converts between units", test_temperature_converts_between_units},
		{"temperature formats signed values", test_temperature_formats_signed_values},
		{"temperature rejects invalid destination", test_temperature_format_rejects_invalid_destination},
		{"byte codec round-trips signed values", test_byte_codec_round_trips_signed_values},
		{"byte codec uses little-endian order", test_byte_codec_uses_little_endian_order},
		{"calendar time converts Unix epoch", test_calendar_time_converts_unix_epoch},
		{"calendar time handles leap day", test_calendar_time_handles_leap_day_and_milliseconds},
		{"calendar time rejects unsupported range", test_calendar_time_rejects_unsupported_range},
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
