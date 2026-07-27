#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "app_settings.h"
#include "unity.h"

#define TEST_EEPROM_SIZE 5U

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

static void test_app_settings_restore_saved_values(void)
{
	int16_t min_temp = 0;
	int16_t max_temp = 0;
	temperature_unit_t unit = TEMPERATURE_UNIT_CELSIUS;

	TEST_ASSERT_TRUE(
		app_settings_save_min_temperature(&eeprom, -123) == AT24C256_OK);
	TEST_ASSERT_TRUE(
		app_settings_save_max_temperature(&eeprom, 456) == AT24C256_OK);
	TEST_ASSERT_TRUE(
		app_settings_save_temperature_unit(
			&eeprom,
			TEMPERATURE_UNIT_FAHRENHEIT) == AT24C256_OK);

	TEST_ASSERT_TRUE(app_settings_load(
		&eeprom,
		&min_temp,
		&max_temp,
		&unit));
	TEST_ASSERT_TRUE(min_temp == -123);
	TEST_ASSERT_TRUE(max_temp == 456);
	TEST_ASSERT_TRUE(unit == TEMPERATURE_UNIT_FAHRENHEIT);
}

static void test_legacy_settings_keep_default_unit(void)
{
	int16_t min_temp = 0;
	int16_t max_temp = 0;
	temperature_unit_t unit = TEMPERATURE_UNIT_CELSIUS;

	TEST_ASSERT_TRUE(
		app_settings_save_min_temperature(&eeprom, 100) == AT24C256_OK);
	TEST_ASSERT_TRUE(
		app_settings_save_max_temperature(&eeprom, 300) == AT24C256_OK);

	TEST_ASSERT_TRUE(app_settings_load(
		&eeprom,
		&min_temp,
		&max_temp,
		&unit));
	TEST_ASSERT_TRUE(min_temp == 100);
	TEST_ASSERT_TRUE(max_temp == 300);
	TEST_ASSERT_TRUE(unit == TEMPERATURE_UNIT_CELSIUS);
}

static void test_invalid_stored_unit_keeps_default(void)
{
	int16_t min_temp = 100;
	int16_t max_temp = 300;
	temperature_unit_t unit = TEMPERATURE_UNIT_CELSIUS;

	eeprom_storage[4] = 0x7FU;

	TEST_ASSERT_TRUE(!app_settings_load(
		&eeprom,
		&min_temp,
		&max_temp,
		&unit));
	TEST_ASSERT_TRUE(unit == TEMPERATURE_UNIT_CELSIUS);
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_app_settings_restore_saved_values);
	RUN_TEST(test_legacy_settings_keep_default_unit);
	RUN_TEST(test_invalid_stored_unit_keeps_default);
	return UNITY_END();
}
