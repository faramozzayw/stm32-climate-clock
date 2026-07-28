#ifndef INC_DRIVERS_BMX280_HPP_
#define INC_DRIVERS_BMX280_HPP_

#include "stm32l0xx_hal.h"

#include <cstdint>
#include <optional>

namespace climate_clock
{
enum class Bmx280Address : std::uint8_t
{
	primary = 0x76U,
	secondary = 0x77U,
};

enum class Bmx280Model : std::uint8_t
{
	unknown,
	bmp280,
	bme280,
};

enum class Bmx280Status : std::uint8_t
{
	ok,
	communication_error,
	unsupported_device,
	calibration_error,
	not_initialized,
	data_unavailable,
	timeout,
};

struct Bmx280Measurement
{
	std::int32_t temperature_centi_celsius{};
	std::uint32_t pressure_pascal{};
	std::optional<std::uint32_t> humidity_milli_percent{};
};

class Bmx280
{
  public:
	Bmx280(
		I2C_HandleTypeDef &i2c,
		Bmx280Address address = Bmx280Address::primary,
		std::uint32_t timeout_ms = 100U);

	[[nodiscard]] Bmx280Status initialize();
	[[nodiscard]] Bmx280Status read(Bmx280Measurement &measurement);
	[[nodiscard]] Bmx280Model model() const;

  private:
	enum class Register : std::uint8_t;

	struct Calibration
	{
		std::uint16_t t1{};
		std::int16_t t2{};
		std::int16_t t3{};
		std::uint16_t p1{};
		std::int16_t p2{};
		std::int16_t p3{};
		std::int16_t p4{};
		std::int16_t p5{};
		std::int16_t p6{};
		std::int16_t p7{};
		std::int16_t p8{};
		std::int16_t p9{};
		std::uint8_t h1{};
		std::int16_t h2{};
		std::uint8_t h3{};
		std::int16_t h4{};
		std::int16_t h5{};
		std::int16_t h6{};
	};

	struct RawMeasurement
	{
		std::int32_t pressure{};
		std::int32_t temperature{};
		std::int32_t humidity{};
	};

	[[nodiscard]] Bmx280Status initialize_device();
	[[nodiscard]] Bmx280Status read_raw_measurement(
		RawMeasurement &measurement) const;
	[[nodiscard]] Bmx280Status read_register(
		Register register_address,
		std::uint8_t &value) const;
	[[nodiscard]] Bmx280Status read_registers(
		Register first_register,
		std::uint8_t *data,
		std::uint16_t length) const;
	[[nodiscard]] Bmx280Status write_register(
		Register register_address,
		std::uint8_t value) const;
	[[nodiscard]] Bmx280Status wait_for_calibration_copy() const;
	[[nodiscard]] Bmx280Status read_calibration();
	[[nodiscard]] Bmx280Status configure() const;

	[[nodiscard]] std::int32_t compensate_temperature(
		std::int32_t raw_temperature,
		std::int32_t &fine_temperature) const;
	[[nodiscard]] std::uint32_t compensate_pressure(
		std::int32_t raw_pressure,
		std::int32_t fine_temperature) const;
	[[nodiscard]] std::uint32_t compensate_humidity(
		std::int32_t raw_humidity,
		std::int32_t fine_temperature) const;

	I2C_HandleTypeDef &i2c_;
	std::uint16_t device_address_;
	std::uint32_t timeout_ms_;
	Bmx280Model model_{Bmx280Model::unknown};
	Calibration calibration_{};
};
} // namespace climate_clock

#endif /* INC_DRIVERS_BMX280_HPP_ */
