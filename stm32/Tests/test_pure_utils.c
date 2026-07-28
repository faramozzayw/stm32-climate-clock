#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "utils/byte_codec.h"
#include "utils/calendar_time.h"
#include "utils/temperature.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static void test_temperature_converts_between_units(void)
{
	TEST_ASSERT_TRUE(celsius_to_fahrenheit_fixed(0) == 320);
	TEST_ASSERT_TRUE(celsius_to_fahrenheit_fixed(100) == 500);
	TEST_ASSERT_TRUE(celsius_to_fahrenheit_fixed(-400) == -400);
}

static void test_temperature_rounds_hundredths_to_tenths(void)
{
	TEST_ASSERT_TRUE(centi_celsius_to_tenths(2504) == 250);
	TEST_ASSERT_TRUE(centi_celsius_to_tenths(2505) == 251);
	TEST_ASSERT_TRUE(centi_celsius_to_tenths(-4) == 0);
	TEST_ASSERT_TRUE(centi_celsius_to_tenths(-5) == -1);
}

static void test_temperature_formats_signed_values(void)
{
	char buffer[16];

	TEST_ASSERT_TRUE(fixed_temperature_to_string(
		buffer, sizeof(buffer), 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	TEST_ASSERT_TRUE(strcmp(buffer, "12.3 C") == 0);

	TEST_ASSERT_TRUE(fixed_temperature_to_string(
		buffer, sizeof(buffer), -5, TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL));
	TEST_ASSERT_TRUE(strcmp(buffer, "-0.5 F") == 0);

	TEST_ASSERT_TRUE(fixed_temperature_to_string(
		buffer, sizeof(buffer), INT16_MIN, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	TEST_ASSERT_TRUE(strcmp(buffer, "-3276.8 C") == 0);
}

static void test_temperature_format_rejects_invalid_destination(void)
{
	char buffer[5];

	TEST_ASSERT_TRUE(!fixed_temperature_to_string(
		NULL, sizeof(buffer), 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	TEST_ASSERT_TRUE(!fixed_temperature_to_string(
		buffer, 0U, 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
	TEST_ASSERT_TRUE(!fixed_temperature_to_string(
		buffer, sizeof(buffer), 123, TEMPERATURE_UNIT_CELSIUS_SYMBOL));
}

static void test_byte_codec_round_trips_signed_values(void)
{
	const int16_t values[] = {0, 1, -1, INT16_MAX, INT16_MIN, 0x1234};
	size_t pos;

	for (pos = 0U; pos < ARRAY_SIZE(values); pos++)
	{
		uint8_t encoded[2];

		write_int16_le(encoded, values[pos]);
		TEST_ASSERT_TRUE(read_int16_le(encoded) == values[pos]);
	}
}

static void test_byte_codec_uses_little_endian_order(void)
{
	uint8_t encoded[2];

	write_int16_le(encoded, 0x1234);
	TEST_ASSERT_TRUE(encoded[0] == 0x34U);
	TEST_ASSERT_TRUE(encoded[1] == 0x12U);
	TEST_ASSERT_TRUE(read_uint16_le(encoded) == 0x1234U);
}

static void test_byte_codec_decodes_signed_12_bit_values(void)
{
	TEST_ASSERT_TRUE(decode_int12(0x0000U) == 0);
	TEST_ASSERT_TRUE(decode_int12(0x07FFU) == 2047);
	TEST_ASSERT_TRUE(decode_int12(0x0800U) == -2048);
	TEST_ASSERT_TRUE(decode_int12(0x0FFFU) == -1);
}

static void test_byte_codec_reads_unsigned_20_bit_big_endian_values(void)
{
	const uint8_t encoded[] = {0xABU, 0xCDU, 0xE0U};

	TEST_ASSERT_TRUE(read_uint20_be(encoded) == 0xABCDEU);
}

static void test_calendar_time_converts_unix_epoch(void)
{
	calendar_time_t time;

	TEST_ASSERT_TRUE(calendar_time_from_unix_ms(946684800000ULL, &time));
	TEST_ASSERT_TRUE(time.year == 2000U);
	TEST_ASSERT_TRUE(time.month == 1U);
	TEST_ASSERT_TRUE(time.day == 1U);
	TEST_ASSERT_TRUE(time.weekday == 7U);
	TEST_ASSERT_TRUE(time.hour == 0U);
	TEST_ASSERT_TRUE(time.minute == 0U);
	TEST_ASSERT_TRUE(time.second == 0U);
}

static void test_calendar_time_handles_leap_day_and_milliseconds(void)
{
	calendar_time_t time;

	TEST_ASSERT_TRUE(calendar_time_from_unix_ms(951827696789ULL, &time));
	TEST_ASSERT_TRUE(time.year == 2000U);
	TEST_ASSERT_TRUE(time.month == 2U);
	TEST_ASSERT_TRUE(time.day == 29U);
	TEST_ASSERT_TRUE(time.weekday == 3U);
	TEST_ASSERT_TRUE(time.hour == 12U);
	TEST_ASSERT_TRUE(time.minute == 34U);
	TEST_ASSERT_TRUE(time.second == 56U);
}

static void test_calendar_time_rejects_unsupported_range(void)
{
	calendar_time_t time = {.year = 1234U};

	TEST_ASSERT_TRUE(!calendar_time_from_unix_ms(946684799999ULL, &time));
	TEST_ASSERT_TRUE(time.year == 1234U);
	TEST_ASSERT_TRUE(!calendar_time_from_unix_ms(4102444800000ULL, &time));
	TEST_ASSERT_TRUE(time.year == 1234U);
	TEST_ASSERT_TRUE(!calendar_time_from_unix_ms(946684800000ULL, NULL));
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
	RUN_TEST(test_temperature_converts_between_units);
	RUN_TEST(test_temperature_rounds_hundredths_to_tenths);
	RUN_TEST(test_temperature_formats_signed_values);
	RUN_TEST(test_temperature_format_rejects_invalid_destination);
	RUN_TEST(test_byte_codec_round_trips_signed_values);
	RUN_TEST(test_byte_codec_uses_little_endian_order);
	RUN_TEST(test_byte_codec_decodes_signed_12_bit_values);
	RUN_TEST(test_byte_codec_reads_unsigned_20_bit_big_endian_values);
	RUN_TEST(test_calendar_time_converts_unix_epoch);
	RUN_TEST(test_calendar_time_handles_leap_day_and_milliseconds);
	RUN_TEST(test_calendar_time_rejects_unsupported_range);
	return UNITY_END();
}
