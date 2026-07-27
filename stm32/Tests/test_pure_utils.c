#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "utils/byte_codec.h"
#include "utils/byte_ring_buffer.h"
#include "utils/calendar_time.h"
#include "utils/temperature.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static void test_temperature_converts_to_fixed_point(void)
{
	TEST_ASSERT_TRUE(tempToFixed(12.34f) == 123);
	TEST_ASSERT_TRUE(tempToFixed(12.35f) == 124);
	TEST_ASSERT_TRUE(tempToFixed(-0.05f) == -1);
}

static void test_temperature_converts_between_units(void)
{
	TEST_ASSERT_TRUE(celsius_to_fahrenheit_fixed(0) == 320);
	TEST_ASSERT_TRUE(celsius_to_fahrenheit_fixed(100) == 500);
	TEST_ASSERT_TRUE(celsius_to_fahrenheit_fixed(-400) == -400);
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

static void test_byte_ring_buffer_validates_initialization(void)
{
	byte_ring_buffer_t buffer = {0};
	uint8_t storage[2];
	uint8_t byte;

	TEST_ASSERT_TRUE(!byte_ring_buffer_init(NULL, storage, sizeof(storage)));
	TEST_ASSERT_TRUE(!byte_ring_buffer_init(&buffer, NULL, sizeof(storage)));
	TEST_ASSERT_TRUE(!byte_ring_buffer_init(&buffer, storage, 1U));
	TEST_ASSERT_TRUE(byte_ring_buffer_is_empty(&buffer));
	TEST_ASSERT_TRUE(!byte_ring_buffer_push(&buffer, 1U));
	TEST_ASSERT_TRUE(!byte_ring_buffer_pop(&buffer, &byte));
}

static void test_byte_ring_buffer_preserves_fifo_order(void)
{
	byte_ring_buffer_t buffer;
	uint8_t storage[4];
	uint8_t byte;

	TEST_ASSERT_TRUE(byte_ring_buffer_init(&buffer, storage, sizeof(storage)));
	TEST_ASSERT_TRUE(byte_ring_buffer_is_empty(&buffer));
	TEST_ASSERT_TRUE(byte_ring_buffer_push(&buffer, 10U));
	TEST_ASSERT_TRUE(byte_ring_buffer_push(&buffer, 20U));
	TEST_ASSERT_TRUE(byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(byte == 10U);
	TEST_ASSERT_TRUE(byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(byte == 20U);
	TEST_ASSERT_TRUE(byte_ring_buffer_is_empty(&buffer));
	TEST_ASSERT_TRUE(!byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(!byte_ring_buffer_pop(&buffer, NULL));
}

static void test_byte_ring_buffer_rejects_full_and_wraps(void)
{
	byte_ring_buffer_t buffer;
	uint8_t storage[4];
	uint8_t byte;

	TEST_ASSERT_TRUE(byte_ring_buffer_init(&buffer, storage, sizeof(storage)));
	TEST_ASSERT_TRUE(byte_ring_buffer_push(&buffer, 1U));
	TEST_ASSERT_TRUE(byte_ring_buffer_push(&buffer, 2U));
	TEST_ASSERT_TRUE(byte_ring_buffer_push(&buffer, 3U));
	TEST_ASSERT_TRUE(!byte_ring_buffer_push(&buffer, 4U));

	TEST_ASSERT_TRUE(byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(byte == 1U);
	TEST_ASSERT_TRUE(byte_ring_buffer_push(&buffer, 4U));

	TEST_ASSERT_TRUE(byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(byte == 2U);
	TEST_ASSERT_TRUE(byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(byte == 3U);
	TEST_ASSERT_TRUE(byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(byte == 4U);
	TEST_ASSERT_TRUE(byte_ring_buffer_is_empty(&buffer));

	TEST_ASSERT_TRUE(byte_ring_buffer_push(&buffer, 5U));
	TEST_ASSERT_TRUE(byte_ring_buffer_pop(&buffer, &byte));
	TEST_ASSERT_TRUE(byte == 5U);
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
	RUN_TEST(test_temperature_converts_to_fixed_point);
	RUN_TEST(test_temperature_converts_between_units);
	RUN_TEST(test_temperature_formats_signed_values);
	RUN_TEST(test_temperature_format_rejects_invalid_destination);
	RUN_TEST(test_byte_codec_round_trips_signed_values);
	RUN_TEST(test_byte_codec_uses_little_endian_order);
	RUN_TEST(test_calendar_time_converts_unix_epoch);
	RUN_TEST(test_calendar_time_handles_leap_day_and_milliseconds);
	RUN_TEST(test_calendar_time_rejects_unsupported_range);
	RUN_TEST(test_byte_ring_buffer_validates_initialization);
	RUN_TEST(test_byte_ring_buffer_preserves_fifo_order);
	RUN_TEST(test_byte_ring_buffer_rejects_full_and_wraps);
	return UNITY_END();
}
