#include "drivers/bmx280.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include "utils/byte_codec.h"

namespace
{
constexpr std::uint8_t bme280_chip_id = 0x60U;
constexpr std::uint8_t bmp280_chip_id = 0x58U;
constexpr std::uint8_t bmp280_sample_id_1 = 0x56U;
constexpr std::uint8_t bmp280_sample_id_2 = 0x57U;
constexpr std::uint8_t reset_command = 0xB6U;
constexpr std::uint8_t calibration_copying_mask = 0x01U;

constexpr std::uint8_t humidity_oversampling_x1 = 0x01U;
constexpr std::uint8_t temperature_oversampling_x1 = 0x01U << 5U;
constexpr std::uint8_t pressure_oversampling_x4 = 0x03U << 2U;
constexpr std::uint8_t normal_mode = 0x03U;
constexpr std::uint8_t standby_1000_ms = 0x05U << 5U;
constexpr std::uint8_t filter_coefficient_4 = 0x02U << 2U;

constexpr std::uint8_t measurement_control =
	temperature_oversampling_x1 |
	pressure_oversampling_x4 |
	normal_mode;
constexpr std::uint8_t configuration =
	standby_1000_ms |
	filter_coefficient_4;

constexpr std::uint8_t calibration_poll_count = 10U;
constexpr std::uint32_t calibration_poll_interval_ms = 2U;

constexpr std::int32_t unavailable_20_bit_measurement = 0x80000;
constexpr std::int32_t unavailable_16_bit_measurement = 0x8000;

climate_clock::Bmx280Model model_from_chip_id(std::uint8_t chip_id)
{
	switch (chip_id)
	{
	case bme280_chip_id:
		return climate_clock::Bmx280Model::bme280;

	case bmp280_chip_id:
	case bmp280_sample_id_1:
	case bmp280_sample_id_2:
		return climate_clock::Bmx280Model::bmp280;

	default:
		return climate_clock::Bmx280Model::unknown;
	}
}
} // namespace

namespace climate_clock
{
enum class Bmx280::Register : std::uint8_t
{
	calibration = 0x88U,
	humidity_calibration = 0xE1U,
	chip_id = 0xD0U,
	reset = 0xE0U,
	humidity_control = 0xF2U,
	status = 0xF3U,
	measurement_control = 0xF4U,
	configuration = 0xF5U,
	measurement = 0xF7U,
};

Bmx280::Bmx280(
	I2C_HandleTypeDef &i2c,
	Bmx280Address address,
	std::uint32_t timeout_ms)
	: i2c_(i2c),
	  device_address_(
		  static_cast<std::uint16_t>(
			  static_cast<std::uint8_t>(address))
		  << 1U),
	  timeout_ms_(timeout_ms)
{
}

Bmx280Status Bmx280::initialize()
{
	model_ = Bmx280Model::unknown;
	const Bmx280Status status = initialize_device();
	if (status != Bmx280Status::ok)
	{
		model_ = Bmx280Model::unknown;
	}

	return status;
}

Bmx280Status Bmx280::initialize_device()
{
	std::uint8_t chip_id{};
	if (const Bmx280Status status =
			read_register(Register::chip_id, chip_id);
		status != Bmx280Status::ok)
	{
		return status;
	}

	model_ = model_from_chip_id(chip_id);
	if (model_ == Bmx280Model::unknown)
	{
		return Bmx280Status::unsupported_device;
	}

	if (const Bmx280Status status =
			write_register(Register::reset, reset_command);
		status != Bmx280Status::ok)
	{
		return status;
	}

	if (const Bmx280Status status = wait_for_calibration_copy();
		status != Bmx280Status::ok)
	{
		return status;
	}

	if (const Bmx280Status status = read_calibration();
		status != Bmx280Status::ok)
	{
		return status;
	}

	return configure();
}

Bmx280Status Bmx280::read(Bmx280Measurement &measurement)
{
	if (model_ == Bmx280Model::unknown)
	{
		return Bmx280Status::not_initialized;
	}

	RawMeasurement raw{};
	if (const Bmx280Status status = read_raw_measurement(raw);
		status != Bmx280Status::ok)
	{
		return status;
	}

	if ((raw.pressure == unavailable_20_bit_measurement) ||
		(raw.temperature == unavailable_20_bit_measurement) ||
		((model_ == Bmx280Model::bme280) &&
			(raw.humidity == unavailable_16_bit_measurement)))
	{
		return Bmx280Status::data_unavailable;
	}

	std::int32_t fine_temperature{};
	measurement.temperature_centi_celsius =
		compensate_temperature(raw.temperature, fine_temperature);
	measurement.pressure_pascal =
		compensate_pressure(raw.pressure, fine_temperature);

	if (model_ == Bmx280Model::bme280)
	{
		measurement.humidity_milli_percent =
			compensate_humidity(raw.humidity, fine_temperature);
	}
	else
	{
		measurement.humidity_milli_percent.reset();
	}

	return Bmx280Status::ok;
}

Bmx280Status Bmx280::read_raw_measurement(
	RawMeasurement &measurement) const
{
	std::array<std::uint8_t, 8U> data{};
	const std::uint16_t length =
		model_ == Bmx280Model::bme280 ? 8U : 6U;
	if (const Bmx280Status status =
			read_registers(Register::measurement, data.data(), length);
		status != Bmx280Status::ok)
	{
		return status;
	}

	measurement.pressure = read_uint20_be(&data[0]);
	measurement.temperature = read_uint20_be(&data[3]);
	measurement.humidity =
		static_cast<std::int32_t>(
			(static_cast<std::uint16_t>(data[6]) << 8U) |
			data[7]);

	return Bmx280Status::ok;
}

Bmx280Model Bmx280::model() const
{
	return model_;
}

Bmx280Status Bmx280::read_register(
	Register register_address,
	std::uint8_t &value) const
{
	return read_registers(register_address, &value, 1U);
}

Bmx280Status Bmx280::read_registers(
	Register first_register,
	std::uint8_t *data,
	std::uint16_t length) const
{
	return HAL_I2C_Mem_Read(
			   &i2c_,
			   device_address_,
			   static_cast<std::uint8_t>(first_register),
			   I2C_MEMADD_SIZE_8BIT,
			   data,
			   length,
			   timeout_ms_) == HAL_OK
			   ? Bmx280Status::ok
			   : Bmx280Status::communication_error;
}

Bmx280Status Bmx280::write_register(
	Register register_address,
	std::uint8_t value) const
{
	return HAL_I2C_Mem_Write(
			   &i2c_,
			   device_address_,
			   static_cast<std::uint8_t>(register_address),
			   I2C_MEMADD_SIZE_8BIT,
			   &value,
			   1U,
			   timeout_ms_) == HAL_OK
			   ? Bmx280Status::ok
			   : Bmx280Status::communication_error;
}

Bmx280Status Bmx280::wait_for_calibration_copy() const
{
	for (std::uint8_t attempt = 0U;
		 attempt < calibration_poll_count;
		 ++attempt)
	{
		std::uint8_t status_register_value{};
		if (const Bmx280Status status =
				read_register(Register::status, status_register_value);
			status != Bmx280Status::ok)
		{
			return status;
		}

		if ((status_register_value & calibration_copying_mask) == 0U)
		{
			return Bmx280Status::ok;
		}

		HAL_Delay(calibration_poll_interval_ms);
	}

	return Bmx280Status::timeout;
}

Bmx280Status Bmx280::read_calibration()
{
	std::array<std::uint8_t, 26U> data{};
	Bmx280Status status = read_registers(
		Register::calibration,
		data.data(),
		static_cast<std::uint16_t>(data.size()));
	if (status != Bmx280Status::ok)
	{
		return status;
	}

	calibration_.t1 = read_uint16_le(&data[0]);
	calibration_.t2 = read_int16_le(&data[2]);
	calibration_.t3 = read_int16_le(&data[4]);
	calibration_.p1 = read_uint16_le(&data[6]);
	calibration_.p2 = read_int16_le(&data[8]);
	calibration_.p3 = read_int16_le(&data[10]);
	calibration_.p4 = read_int16_le(&data[12]);
	calibration_.p5 = read_int16_le(&data[14]);
	calibration_.p6 = read_int16_le(&data[16]);
	calibration_.p7 = read_int16_le(&data[18]);
	calibration_.p8 = read_int16_le(&data[20]);
	calibration_.p9 = read_int16_le(&data[22]);
	calibration_.h1 = data[25];

	if (model_ == Bmx280Model::bme280)
	{
		std::array<std::uint8_t, 7U> humidity_data{};
		status = read_registers(
			Register::humidity_calibration,
			humidity_data.data(),
			static_cast<std::uint16_t>(humidity_data.size()));
		if (status != Bmx280Status::ok)
		{
			return status;
		}

		calibration_.h2 = read_int16_le(&humidity_data[0]);
		calibration_.h3 = humidity_data[2];
		calibration_.h4 = decode_int12(
			static_cast<std::uint16_t>(
				(static_cast<std::uint16_t>(humidity_data[3]) << 4U) |
				(humidity_data[4] & 0x0FU)));
		calibration_.h5 = decode_int12(
			static_cast<std::uint16_t>(
				(static_cast<std::uint16_t>(humidity_data[5]) << 4U) |
				(humidity_data[4] >> 4U)));
		calibration_.h6 =
			humidity_data[6] > INT8_MAX
				? static_cast<std::int16_t>(humidity_data[6]) - 256
				: humidity_data[6];
	}

	if ((calibration_.t1 == 0U) || (calibration_.p1 == 0U))
	{
		return Bmx280Status::calibration_error;
	}

	return Bmx280Status::ok;
}

Bmx280Status Bmx280::configure() const
{
	if (model_ == Bmx280Model::bme280)
	{
		const Bmx280Status status = write_register(
			Register::humidity_control,
			humidity_oversampling_x1);
		if (status != Bmx280Status::ok)
		{
			return status;
		}
	}

	Bmx280Status status =
		write_register(Register::configuration, configuration);
	if (status != Bmx280Status::ok)
	{
		return status;
	}

	return write_register(
		Register::measurement_control,
		measurement_control);
}

std::int32_t Bmx280::compensate_temperature(
	std::int32_t raw_temperature,
	std::int32_t &fine_temperature) const
{
	const std::int32_t t1 = calibration_.t1;
	const std::int32_t t2 = calibration_.t2;
	const std::int32_t t3 = calibration_.t3;

	const std::int32_t linear_correction =
		(((raw_temperature >> 3) - (t1 << 1)) * t2) >> 11;

	const std::int32_t temperature_delta =
		(raw_temperature >> 4) - t1;
	const std::int32_t quadratic_correction =
		(((temperature_delta * temperature_delta) >> 12) * t3) >> 14;

	fine_temperature = linear_correction + quadratic_correction;
	return (fine_temperature * 5 + 128) >> 8;
}

std::uint32_t Bmx280::compensate_pressure(
	std::int32_t raw_pressure,
	std::int32_t fine_temperature) const
{
	const std::int64_t p1 = calibration_.p1;
	const std::int64_t p2 = calibration_.p2;
	const std::int64_t p3 = calibration_.p3;
	const std::int64_t p4 = calibration_.p4;
	const std::int64_t p5 = calibration_.p5;
	const std::int64_t p6 = calibration_.p6;
	const std::int64_t p7 = calibration_.p7;
	const std::int64_t p8 = calibration_.p8;
	const std::int64_t p9 = calibration_.p9;

	// These scales and shifts are defined by Bosch's fixed-point algorithm.
	const std::int64_t temperature_delta =
		static_cast<std::int64_t>(fine_temperature) - 128000;
	const std::int64_t temperature_squared =
		temperature_delta * temperature_delta;

	const std::int64_t pressure_offset =
		(temperature_squared * p6) +
		(temperature_delta * p5 * 131072) +
		(p4 * 34359738368);

	std::int64_t pressure_sensitivity =
		((temperature_squared * p3) >> 8) +
		(temperature_delta * p2 * 4096);
	pressure_sensitivity =
		(((static_cast<std::int64_t>(1) << 47) +
			 pressure_sensitivity) *
			p1) >>
		33;

	if (pressure_sensitivity == 0)
	{
		return 0U;
	}

	std::int64_t pressure_q24_8 = 1048576 - raw_pressure;
	pressure_q24_8 =
		(((pressure_q24_8 << 31) - pressure_offset) * 3125) /
		pressure_sensitivity;

	const std::int64_t quadratic_correction =
		(p9 *
			(pressure_q24_8 >> 13) *
			(pressure_q24_8 >> 13)) >>
		25;
	const std::int64_t linear_correction =
		(p8 * pressure_q24_8) >>
		19;
	pressure_q24_8 =
		((pressure_q24_8 +
			 quadratic_correction +
			 linear_correction) >>
			8) +
		(p7 * 16);

	if (pressure_q24_8 <= 0)
	{
		return 0U;
	}

	const std::uint64_t positive_pressure =
		static_cast<std::uint64_t>(pressure_q24_8);
	return static_cast<std::uint32_t>(
		(positive_pressure + 128U) >> 8U);
}

std::uint32_t Bmx280::compensate_humidity(
	std::int32_t raw_humidity,
	std::int32_t fine_temperature) const
{
	const std::int32_t h1 = calibration_.h1;
	const std::int32_t h2 = calibration_.h2;
	const std::int32_t h3 = calibration_.h3;
	const std::int32_t h4 = calibration_.h4;
	const std::int32_t h5 = calibration_.h5;
	const std::int32_t h6 = calibration_.h6;

	// These scales and shifts are defined by Bosch's fixed-point algorithm.
	const std::int32_t temperature_delta =
		fine_temperature - 76800;
	const std::int32_t humidity_offset =
		((raw_humidity << 14) -
			(h4 * 1048576) -
			(h5 * temperature_delta) +
			16384) >>
		15;
	const std::int32_t temperature_correction =
		(((temperature_delta * h6) >>
			 10) *
			(((temperature_delta * h3) >>
				 11) +
				32768)) >>
		10;
	const std::int32_t humidity_sensitivity =
		(((temperature_correction + 2097152) *
			 h2) +
			8192) >>
		14;

	std::int32_t humidity =
		humidity_offset * humidity_sensitivity;
	const std::int32_t non_linearity_correction =
		(((((humidity >> 15) * (humidity >> 15)) >> 7) * h1) >> 4);
	humidity -= non_linearity_correction;
	humidity = std::clamp(
		humidity,
		std::int32_t{0},
		std::int32_t{419430400});

	const std::uint32_t humidity_q22_10 =
		static_cast<std::uint32_t>(humidity >> 12);
	return static_cast<std::uint32_t>(
		(static_cast<std::uint64_t>(humidity_q22_10) * 1000U + 512U) >>
		10U);
}
} // namespace climate_clock
