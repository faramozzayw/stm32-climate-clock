#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

extern "C"
{
#include "unity.h"
}

#include "app/settings.hpp"

using climate_clock::Settings;

#define TEST_EEPROM_SIZE 9U

static uint8_t eeprom_storage[TEST_EEPROM_SIZE];
static at24c256_t eeprom;

AT24C256_Status at24c256_read(
	const at24c256_t *device,
	uint16_t address,
	uint8_t *data,
	size_t length)
{
	(void)device;

	if (((size_t)address + length) > sizeof(eeprom_storage))
	{
		return AT24C256_OUT_OF_RANGE;
	}

	memcpy(data, &eeprom_storage[address], length);
	return AT24C256_OK;
}

AT24C256_Status at24c256_write(
	const at24c256_t *device,
	uint16_t address,
	const uint8_t *data,
	size_t length)
{
	(void)device;

	if (((size_t)address + length) > sizeof(eeprom_storage))
	{
		return AT24C256_OUT_OF_RANGE;
	}

	memcpy(&eeprom_storage[address], data, length);
	return AT24C256_OK;
}

AT24C256_Status at24c256_read_byte(
	const at24c256_t *device,
	uint16_t address,
	uint8_t *value)
{
	return at24c256_read(device, address, value, 1U);
}

AT24C256_Status at24c256_write_byte(
	const at24c256_t *device,
	uint16_t address,
	uint8_t value)
{
	return at24c256_write(device, address, &value, 1U);
}

void setUp(void)
{
	memset(eeprom_storage, 0xFF, sizeof(eeprom_storage));
	memset(&eeprom, 0, sizeof(eeprom));
}

void tearDown(void)
{
}

static void test_settings_restore_saved_values(void)
{
	Settings settings{eeprom};
	int16_t min_temp = 0;
	int16_t max_temp = 0;
	uint16_t min_humidity = 0U;
	uint16_t max_humidity = 0U;
	temperature_unit_t unit = TEMPERATURE_UNIT_CELSIUS;

	TEST_ASSERT_EQUAL(AT24C256_OK, settings.save_min_temperature(-123));
	TEST_ASSERT_EQUAL(AT24C256_OK, settings.save_max_temperature(456));
	TEST_ASSERT_EQUAL(AT24C256_OK, settings.save_min_humidity(300U));
	TEST_ASSERT_EQUAL(AT24C256_OK, settings.save_max_humidity(650U));
	TEST_ASSERT_EQUAL(
		AT24C256_OK,
		settings.save_temperature_unit(TEMPERATURE_UNIT_FAHRENHEIT));

	TEST_ASSERT_TRUE(settings.load(
		min_temp,
		max_temp,
		min_humidity,
		max_humidity,
		unit));
	TEST_ASSERT_TRUE(min_temp == -123);
	TEST_ASSERT_TRUE(max_temp == 456);
	TEST_ASSERT_EQUAL_UINT16(300U, min_humidity);
	TEST_ASSERT_EQUAL_UINT16(650U, max_humidity);
	TEST_ASSERT_TRUE(unit == TEMPERATURE_UNIT_FAHRENHEIT);
}

static void test_legacy_settings_keep_default_unit(void)
{
	Settings settings{eeprom};
	int16_t min_temp = 0;
	int16_t max_temp = 0;
	uint16_t min_humidity = 300U;
	uint16_t max_humidity = 600U;
	temperature_unit_t unit = TEMPERATURE_UNIT_CELSIUS;

	TEST_ASSERT_EQUAL(AT24C256_OK, settings.save_min_temperature(100));
	TEST_ASSERT_EQUAL(AT24C256_OK, settings.save_max_temperature(300));

	TEST_ASSERT_TRUE(settings.load(
		min_temp,
		max_temp,
		min_humidity,
		max_humidity,
		unit));
	TEST_ASSERT_TRUE(min_temp == 100);
	TEST_ASSERT_TRUE(max_temp == 300);
	TEST_ASSERT_EQUAL_UINT16(300U, min_humidity);
	TEST_ASSERT_EQUAL_UINT16(600U, max_humidity);
	TEST_ASSERT_TRUE(unit == TEMPERATURE_UNIT_CELSIUS);
}

static void test_invalid_stored_unit_keeps_default(void)
{
	Settings settings{eeprom};
	int16_t min_temp = 100;
	int16_t max_temp = 300;
	uint16_t min_humidity = 300U;
	uint16_t max_humidity = 600U;
	temperature_unit_t unit = TEMPERATURE_UNIT_CELSIUS;

	eeprom_storage[4] = 0x7FU;

	TEST_ASSERT_FALSE(settings.load(
		min_temp,
		max_temp,
		min_humidity,
		max_humidity,
		unit));
	TEST_ASSERT_TRUE(unit == TEMPERATURE_UNIT_CELSIUS);
}

static void test_invalid_stored_humidity_keeps_default(void)
{
	Settings settings{eeprom};
	int16_t min_temp = 100;
	int16_t max_temp = 300;
	uint16_t min_humidity = 300U;
	uint16_t max_humidity = 600U;
	temperature_unit_t unit = TEMPERATURE_UNIT_CELSIUS;

	eeprom_storage[5] = 0xE9U;
	eeprom_storage[6] = 0x03U;

	TEST_ASSERT_FALSE(settings.load(
		min_temp,
		max_temp,
		min_humidity,
		max_humidity,
		unit));
	TEST_ASSERT_EQUAL_UINT16(300U, min_humidity);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_settings_restore_saved_values);
	RUN_TEST(test_legacy_settings_keep_default_unit);
	RUN_TEST(test_invalid_stored_unit_keeps_default);
	RUN_TEST(test_invalid_stored_humidity_keeps_default);
	return UNITY_END();
}
