extern "C"
{
#include "unity.h"
}

#include "drivers/bmx280.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

using climate_clock::Bmx280;
using climate_clock::Bmx280Address;
using climate_clock::Bmx280Measurement;
using climate_clock::Bmx280Model;
using climate_clock::Bmx280Status;

namespace
{
constexpr std::uint8_t chip_id_register = 0xD0U;
constexpr std::uint8_t status_register = 0xF3U;
constexpr std::uint8_t humidity_control_register = 0xF2U;
constexpr std::uint8_t measurement_control_register = 0xF4U;
constexpr std::uint8_t configuration_register = 0xF5U;
constexpr std::uint8_t measurement_register = 0xF7U;

std::uint8_t registers[256]{};
bool fail_i2c{};
std::uint16_t last_device_address{};
std::uint32_t delay_call_count{};

void write_u16_le(std::uint8_t address, std::uint16_t value)
{
	registers[address] = static_cast<std::uint8_t>(value);
	registers[address + 1U] = static_cast<std::uint8_t>(value >> 8U);
}

void write_i16_le(std::uint8_t address, std::int16_t value)
{
	write_u16_le(address, static_cast<std::uint16_t>(value));
}

void write_i12(
	std::uint8_t msb_address,
	std::uint8_t shared_address,
	std::int16_t value,
	bool low_nibble)
{
	const std::uint16_t encoded =
		static_cast<std::uint16_t>(value) & 0x0FFFU;
	registers[msb_address] =
		static_cast<std::uint8_t>(encoded >> 4U);

	if (low_nibble)
	{
		registers[shared_address] =
			static_cast<std::uint8_t>(
				(registers[shared_address] & 0xF0U) |
				(encoded & 0x0FU));
	}
	else
	{
		registers[shared_address] =
			static_cast<std::uint8_t>(
				(registers[shared_address] & 0x0FU) |
				((encoded & 0x0FU) << 4U));
	}
}

void write_i20_be(std::uint8_t address, std::int32_t value)
{
	const std::uint32_t encoded = static_cast<std::uint32_t>(value);
	registers[address] =
		static_cast<std::uint8_t>(encoded >> 12U);
	registers[address + 1U] =
		static_cast<std::uint8_t>(encoded >> 4U);
	registers[address + 2U] =
		static_cast<std::uint8_t>(encoded << 4U);
}

void load_pressure_and_temperature_calibration()
{
	write_u16_le(0x88U, 27504U);
	write_i16_le(0x8AU, 26435);
	write_i16_le(0x8CU, -1000);
	write_u16_le(0x8EU, 36477U);
	write_i16_le(0x90U, -10685);
	write_i16_le(0x92U, 3024);
	write_i16_le(0x94U, 2855);
	write_i16_le(0x96U, 140);
	write_i16_le(0x98U, -7);
	write_i16_le(0x9AU, 15500);
	write_i16_le(0x9CU, -14600);
	write_i16_le(0x9EU, 6000);
}

void load_humidity_calibration()
{
	registers[0xA1U] = 75U;
	write_i16_le(0xE1U, 362);
	registers[0xE3U] = 0U;
	write_i12(0xE4U, 0xE5U, 334, true);
	write_i12(0xE6U, 0xE5U, 50, false);
	registers[0xE7U] = 30U;
}

void load_raw_measurement(std::uint16_t humidity)
{
	write_i20_be(measurement_register, 415148);
	write_i20_be(measurement_register + 3U, 519888);
	registers[measurement_register + 6U] =
		static_cast<std::uint8_t>(humidity >> 8U);
	registers[measurement_register + 7U] =
		static_cast<std::uint8_t>(humidity);
}
} // namespace

extern "C" HAL_StatusTypeDef HAL_I2C_Mem_Read(
	I2C_HandleTypeDef *,
	std::uint16_t address,
	std::uint16_t memory_address,
	std::uint16_t,
	std::uint8_t *data,
	std::uint16_t size,
	std::uint32_t)
{
	last_device_address = address;
	if (fail_i2c)
	{
		return HAL_ERROR;
	}

	std::memcpy(data, &registers[memory_address], size);
	return HAL_OK;
}

extern "C" HAL_StatusTypeDef HAL_I2C_Mem_Write(
	I2C_HandleTypeDef *,
	std::uint16_t address,
	std::uint16_t memory_address,
	std::uint16_t,
	std::uint8_t *data,
	std::uint16_t size,
	std::uint32_t)
{
	last_device_address = address;
	if (fail_i2c)
	{
		return HAL_ERROR;
	}

	std::memcpy(&registers[memory_address], data, size);
	return HAL_OK;
}

extern "C" void HAL_Delay(std::uint32_t)
{
	++delay_call_count;
}

void setUp()
{
	std::memset(registers, 0, sizeof(registers));
	fail_i2c = false;
	last_device_address = 0U;
	delay_call_count = 0U;
	load_pressure_and_temperature_calibration();
}

void tearDown()
{
}

static void test_bmp280_is_detected_and_configured()
{
	I2C_HandleTypeDef i2c{};
	registers[chip_id_register] = 0x58U;
	Bmx280 sensor{i2c};

	TEST_ASSERT_TRUE(sensor.initialize() == Bmx280Status::ok);
	TEST_ASSERT_TRUE(sensor.model() == Bmx280Model::bmp280);
	TEST_ASSERT_EQUAL_HEX16(0xECU, last_device_address);
	TEST_ASSERT_EQUAL_HEX8(0xA8U, registers[configuration_register]);
	TEST_ASSERT_EQUAL_HEX8(0x2FU, registers[measurement_control_register]);
	TEST_ASSERT_EQUAL_HEX8(0x00U, registers[humidity_control_register]);
}

static void test_bme280_is_detected_and_enables_humidity()
{
	I2C_HandleTypeDef i2c{};
	registers[chip_id_register] = 0x60U;
	load_humidity_calibration();
	Bmx280 sensor{i2c, Bmx280Address::secondary};

	TEST_ASSERT_TRUE(sensor.initialize() == Bmx280Status::ok);
	TEST_ASSERT_TRUE(sensor.model() == Bmx280Model::bme280);
	TEST_ASSERT_EQUAL_HEX16(0xEEU, last_device_address);
	TEST_ASSERT_EQUAL_HEX8(0x01U, registers[humidity_control_register]);
}

static void test_bmp280_compensates_datasheet_measurement()
{
	I2C_HandleTypeDef i2c{};
	registers[chip_id_register] = 0x58U;
	load_raw_measurement(0U);
	Bmx280 sensor{i2c};
	Bmx280Measurement measurement{};

	TEST_ASSERT_TRUE(sensor.initialize() == Bmx280Status::ok);
	TEST_ASSERT_TRUE(sensor.read(measurement) == Bmx280Status::ok);
	TEST_ASSERT_EQUAL_INT32(2508, measurement.temperature_centi_celsius);
	TEST_ASSERT_EQUAL_UINT32(100653U, measurement.pressure_pascal);
	TEST_ASSERT_FALSE(measurement.humidity_milli_percent.has_value());
}

static void test_bme280_compensates_humidity()
{
	I2C_HandleTypeDef i2c{};
	registers[chip_id_register] = 0x60U;
	load_humidity_calibration();
	load_raw_measurement(32257U);
	Bmx280 sensor{i2c};
	Bmx280Measurement measurement{};

	TEST_ASSERT_TRUE(sensor.initialize() == Bmx280Status::ok);
	TEST_ASSERT_TRUE(sensor.read(measurement) == Bmx280Status::ok);
	TEST_ASSERT_TRUE(measurement.humidity_milli_percent.has_value());
	TEST_ASSERT_UINT32_WITHIN(
		100U,
		60075U,
		measurement.humidity_milli_percent.value());
}

static void test_driver_reports_unsupported_chip()
{
	I2C_HandleTypeDef i2c{};
	registers[chip_id_register] = 0x61U;
	Bmx280 sensor{i2c};

	TEST_ASSERT_TRUE(
		sensor.initialize() == Bmx280Status::unsupported_device);
	TEST_ASSERT_TRUE(sensor.model() == Bmx280Model::unknown);
}

static void test_driver_reports_i2c_failure()
{
	I2C_HandleTypeDef i2c{};
	fail_i2c = true;
	Bmx280 sensor{i2c};

	TEST_ASSERT_TRUE(
		sensor.initialize() == Bmx280Status::communication_error);
}

static void test_read_requires_initialization()
{
	I2C_HandleTypeDef i2c{};
	Bmx280 sensor{i2c};
	Bmx280Measurement measurement{};

	TEST_ASSERT_TRUE(
		sensor.read(measurement) == Bmx280Status::not_initialized);
}

static void test_initialization_times_out_while_calibration_is_copying()
{
	I2C_HandleTypeDef i2c{};
	registers[chip_id_register] = 0x58U;
	registers[status_register] = 0x01U;
	Bmx280 sensor{i2c};

	TEST_ASSERT_TRUE(sensor.initialize() == Bmx280Status::timeout);
	TEST_ASSERT_EQUAL_UINT32(10U, delay_call_count);
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_bmp280_is_detected_and_configured);
	RUN_TEST(test_bme280_is_detected_and_enables_humidity);
	RUN_TEST(test_bmp280_compensates_datasheet_measurement);
	RUN_TEST(test_bme280_compensates_humidity);
	RUN_TEST(test_driver_reports_unsupported_chip);
	RUN_TEST(test_driver_reports_i2c_failure);
	RUN_TEST(test_read_requires_initialization);
	RUN_TEST(test_initialization_times_out_while_calibration_is_copying);
	return UNITY_END();
}
