#include "app/app.hpp"

#include <cstdio>

#include "telemetry.hpp"
#include "utils/calendar_time.h"
#include "utils/temperature.h"

namespace climate_clock
{
namespace
{
constexpr std::uint32_t connection_led_pwm_channel = TIM_CHANNEL_4;
constexpr std::uint32_t connection_led_breathe_half_cycle_ms = 200U;
constexpr std::uint32_t connection_led_min_duty_percent = 10U;
constexpr std::uint32_t connection_led_max_duty_percent = 65U;

std::uint32_t duty_from_percent(
	std::uint32_t timer_period,
	std::uint32_t percent)
{
	return (timer_period * percent) / 100U;
}

std::uint32_t breathing_duty(std::uint32_t timer_period)
{
	const auto half_cycle = connection_led_breathe_half_cycle_ms;
	const auto full_cycle = half_cycle * 2U;
	const auto min_duty = duty_from_percent(
		timer_period,
		connection_led_min_duty_percent);
	const auto max_duty = duty_from_percent(
		timer_period,
		connection_led_max_duty_percent);
	auto phase = HAL_GetTick() % full_cycle;

	if (phase > half_cycle)
	{
		phase = full_cycle - phase;
	}

	return min_duty + (((max_duty - min_duty) * phase) / half_cycle);
}

std::uint32_t connection_led_duty(
	ble_connection_state_t state,
	std::uint32_t timer_period)
{
	switch (state)
	{
	case BLE_CONNECTION_STATE_DISCONNECTED:
		return 0U;

	case BLE_CONNECTION_STATE_CONNECTED:
		return timer_period;

	case BLE_CONNECTION_STATE_CONNECTING:
	case BLE_CONNECTION_STATE_DISCONNECTING:
		return breathing_duty(timer_period);

	default:
		return 0U;
	}
}
} // namespace

App::App(
	lcd1602_t &lcd,
	hw479_t &temperature_indicator,
	ds3231_t &rtc,
	Bmx280 &environment_sensor,
	at24c256_t &eeprom,
	I2C_HandleTypeDef &eeprom_i2c,
	uart_command_receiver_t &command_receiver,
	hal_uart_transport_t &telemetry_transport)
	: indicator_driver_(temperature_indicator),
	  rtc_(rtc),
	  environment_sensor_(environment_sensor),
	  eeprom_(eeprom),
	  eeprom_i2c_(eeprom_i2c),
	  command_receiver_(command_receiver),
	  telemetry_transport_(telemetry_transport),
	  screen_(lcd),
	  temperature_indicator_(temperature_indicator),
	  settings_(eeprom)
{
}

bool App::initialize()
{
	screen_.initialize();
	temperature_indicator_.initialize();

	if (HAL_TIM_PWM_Start(
			hw479_get_timer(&indicator_driver_),
			connection_led_pwm_channel) != HAL_OK)
	{
		std::printf("Connection LED PWM start failed\r\n");
		return false;
	}

	set_connection_state(BLE_CONNECTION_STATE_DISCONNECTED);

	const auto sensor_status = environment_sensor_.initialize();
	if (sensor_status != Bmx280Status::ok)
	{
		std::printf(
			"BME/BMP280 initialization failed: %u\r\n",
			static_cast<unsigned int>(sensor_status));
		return false;
	}

	std::printf(
		"%s initialized\r\n",
		environment_sensor_.model() == Bmx280Model::bme280
			? "BME280"
			: "BMP280");

	const auto eeprom_status = at24c256_init(
		&eeprom_,
		&eeprom_i2c_,
		AT24C256_DEFAULT_ADDRESS,
		100U);

	if (eeprom_status == AT24C256_OK)
	{
		eeprom_ready_ = true;

		if (settings_.load(
				min_temperature_,
				max_temperature_,
				min_humidity_tenths_percent_,
				max_humidity_tenths_percent_,
				temperature_unit_))
		{
			std::printf(
				"Loaded settings: temperature=%d..%d, humidity=%u.%u..%u.%u%%, unit=%c\r\n",
				static_cast<int>(min_temperature_),
				static_cast<int>(max_temperature_),
				static_cast<unsigned int>(
					min_humidity_tenths_percent_ / 10U),
				static_cast<unsigned int>(
					min_humidity_tenths_percent_ % 10U),
				static_cast<unsigned int>(
					max_humidity_tenths_percent_ / 10U),
				static_cast<unsigned int>(
					max_humidity_tenths_percent_ % 10U),
				temperature_unit_ == TEMPERATURE_UNIT_FAHRENHEIT
					? TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL
					: TEMPERATURE_UNIT_CELSIUS_SYMBOL);
		}
		else
		{
			std::printf(
				"No valid settings; using defaults: temperature=%d..%d, humidity=%u.%u..%u.%u%%, unit=%c\r\n",
				static_cast<int>(min_temperature_),
				static_cast<int>(max_temperature_),
				static_cast<unsigned int>(
					min_humidity_tenths_percent_ / 10U),
				static_cast<unsigned int>(
					min_humidity_tenths_percent_ % 10U),
				static_cast<unsigned int>(
					max_humidity_tenths_percent_ / 10U),
				static_cast<unsigned int>(
					max_humidity_tenths_percent_ % 10U),
				TEMPERATURE_UNIT_CELSIUS_SYMBOL);
		}
	}
	else
	{
		std::printf(
			"AT24C256 unavailable; using default settings\r\n");
	}

	uart_command_receiver_init(&command_receiver_);
	command_receiver_.values.min_temp = min_temperature_;
	command_receiver_.values.max_temp = max_temperature_;
	command_receiver_.values.min_humidity_tenths_percent =
		min_humidity_tenths_percent_;
	command_receiver_.values.max_humidity_tenths_percent =
		max_humidity_tenths_percent_;
	command_receiver_.values.temperature_unit =
		temperature_unit_ == TEMPERATURE_UNIT_FAHRENHEIT
			? DEVICE_TEMPERATURE_UNIT_FAHRENHEIT
			: DEVICE_TEMPERATURE_UNIT_CELSIUS;

	return true;
}

void App::poll()
{
	uart_command_receiver_poll(&command_receiver_);
	apply_received_messages();
	update_connection_led();
}

void App::update()
{
	std::uint8_t telemetry_frame[telemetry::max_frame_size];
	std::uint16_t telemetry_frame_length;
	Bmx280Measurement measurement{};

	const bool settings_were_pending = settings_telemetry_pending_;
	if (settings_were_pending)
	{
		if (telemetry::encode_settings(
				min_temperature_,
				max_temperature_,
				min_humidity_tenths_percent_,
				max_humidity_tenths_percent_,
				temperature_unit_,
				telemetry_frame,
				telemetry_frame_length) &&
			hal_uart_transport_send(
				&telemetry_transport_,
				telemetry_frame,
				telemetry_frame_length))
		{
			settings_telemetry_pending_ = false;
		}
	}

	const auto time = ds3231_get_time(&rtc_);
	const auto sensor_status = environment_sensor_.read(measurement);
	if (sensor_status != Bmx280Status::ok)
	{
		std::printf(
			"BME/BMP280 read failed: %u\r\n",
			static_cast<unsigned int>(sensor_status));
		return;
	}

	const auto temperature = centi_celsius_to_tenths(
		measurement.temperature_centi_celsius);

	if (!settings_were_pending &&
		telemetry::encode_measurement(
			temperature,
			measurement.humidity_milli_percent,
			telemetry_frame,
			telemetry_frame_length))
	{
		hal_uart_transport_send(
			&telemetry_transport_,
			telemetry_frame,
			telemetry_frame_length);
	}

	screen_.update(
		time,
		temperature,
		temperature_unit_,
		measurement.humidity_milli_percent);
	temperature_indicator_.update(
		temperature,
		min_temperature_,
		max_temperature_);
}

void App::apply_received_messages()
{
	auto &values = command_receiver_.values;

	if (values.min_temp_updated)
	{
		min_temperature_ = values.min_temp;
		settings_telemetry_pending_ = true;
		values.min_temp_updated = false;

		if (eeprom_ready_ &&
			settings_.save_min_temperature(
				min_temperature_) != AT24C256_OK)
		{
			std::printf("Failed to save minimum temperature\r\n");
		}
	}

	if (values.max_temp_updated)
	{
		max_temperature_ = values.max_temp;
		settings_telemetry_pending_ = true;
		values.max_temp_updated = false;

		if (eeprom_ready_ &&
			settings_.save_max_temperature(
				max_temperature_) != AT24C256_OK)
		{
			std::printf("Failed to save maximum temperature\r\n");
		}
	}

	if (values.min_humidity_updated)
	{
		min_humidity_tenths_percent_ =
			values.min_humidity_tenths_percent;
		settings_telemetry_pending_ = true;
		values.min_humidity_updated = false;

		if (eeprom_ready_ &&
			settings_.save_min_humidity(
				min_humidity_tenths_percent_) != AT24C256_OK)
		{
			std::printf("Failed to save minimum humidity\r\n");
		}
	}

	if (values.max_humidity_updated)
	{
		max_humidity_tenths_percent_ =
			values.max_humidity_tenths_percent;
		settings_telemetry_pending_ = true;
		values.max_humidity_updated = false;

		if (eeprom_ready_ &&
			settings_.save_max_humidity(
				max_humidity_tenths_percent_) != AT24C256_OK)
		{
			std::printf("Failed to save maximum humidity\r\n");
		}
	}

	if (values.current_time_updated)
	{
		calendar_time_t time;

		if (calendar_time_from_unix_ms(values.current_time_ms, &time))
		{
			ds3231_set_time(&rtc_, time);
			std::printf(
				"RTC set to %02u:%02u:%02u %02u/%02u/%04u UTC\r\n",
				static_cast<unsigned int>(time.hour),
				static_cast<unsigned int>(time.minute),
				static_cast<unsigned int>(time.second),
				static_cast<unsigned int>(time.day),
				static_cast<unsigned int>(time.month),
				static_cast<unsigned int>(time.year));
		}
		else
		{
			std::printf(
				"SetCurrentTime is outside DS3231 range (2000-2099)\r\n");
		}

		values.current_time_updated = false;
	}

	if (values.temperature_unit_updated)
	{
		const auto unit =
			values.temperature_unit == DEVICE_TEMPERATURE_UNIT_FAHRENHEIT
				? TEMPERATURE_UNIT_FAHRENHEIT
				: TEMPERATURE_UNIT_CELSIUS;

		set_temperature_unit(unit);
		values.temperature_unit_updated = false;
	}

	if (connection_state_ != values.ble_connection_state)
	{
		set_connection_state(values.ble_connection_state);
	}
}

void App::set_temperature_unit(temperature_unit_t unit)
{
	if (temperature_unit_ == unit)
	{
		return;
	}

	temperature_unit_ = unit;
	settings_telemetry_pending_ = true;

	if (eeprom_ready_ &&
		settings_.save_temperature_unit(unit) != AT24C256_OK)
	{
		std::printf("Failed to save temperature unit\r\n");
	}
}

void App::set_connection_state(ble_connection_state_t state)
{
	connection_state_ = state;
	settings_telemetry_pending_ =
		state == BLE_CONNECTION_STATE_CONNECTED;
	update_connection_led();
}

void App::update_connection_led()
{
	auto *timer = hw479_get_timer(&indicator_driver_);
	const auto timer_period = __HAL_TIM_GET_AUTORELOAD(timer) + 1U;
	const auto duty = connection_led_duty(connection_state_, timer_period);

	__HAL_TIM_SET_COMPARE(
		timer,
		connection_led_pwm_channel,
		duty);
}
} // namespace climate_clock
