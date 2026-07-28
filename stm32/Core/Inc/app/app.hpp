#ifndef INC_APP_APP_HPP_
#define INC_APP_APP_HPP_

#include <cstdint>

#include "app/screen.hpp"
#include "app/temperature_indicator.hpp"
#include "app/settings.hpp"
#include "command_receiver/uart_command_receiver.h"
#include "drivers/bmx280.hpp"
#include "drivers/ds3231.h"
#include "platform/hal_uart_transport.h"

namespace climate_clock
{
class App
{
  public:
	static constexpr std::uint32_t update_interval_ms = 250U;

	App(
		lcd1602_t &lcd,
		hw479_t &temperature_indicator,
		ds3231_t &rtc,
		Bmx280 &environment_sensor,
		at24c256_t &eeprom,
		I2C_HandleTypeDef &eeprom_i2c,
		uart_command_receiver_t &command_receiver,
		hal_uart_transport_t &telemetry_transport);

	[[nodiscard]] bool initialize();
	void poll();
	void update();

  private:
	static constexpr std::int16_t default_min_temperature = 100;
	static constexpr std::int16_t default_max_temperature = 300;
	static constexpr std::uint16_t default_min_humidity = 300U;
	static constexpr std::uint16_t default_max_humidity = 600U;

	void apply_received_messages();
	void set_temperature_unit(temperature_unit_t unit);
	void set_connection_state(ble_connection_state_t state);
	void update_connection_led();

	hw479_t &indicator_driver_;
	ds3231_t &rtc_;
	Bmx280 &environment_sensor_;
	at24c256_t &eeprom_;
	I2C_HandleTypeDef &eeprom_i2c_;
	uart_command_receiver_t &command_receiver_;
	hal_uart_transport_t &telemetry_transport_;

	Screen screen_;
	TemperatureIndicator temperature_indicator_;
	Settings settings_;

	std::int16_t min_temperature_{default_min_temperature};
	std::int16_t max_temperature_{default_max_temperature};
	std::uint16_t min_humidity_tenths_percent_{default_min_humidity};
	std::uint16_t max_humidity_tenths_percent_{default_max_humidity};
	temperature_unit_t temperature_unit_{TEMPERATURE_UNIT_CELSIUS};
	ble_connection_state_t connection_state_{
		BLE_CONNECTION_STATE_DISCONNECTED};
	bool eeprom_ready_{false};
	bool limits_telemetry_pending_{false};
};
} // namespace climate_clock

#endif /* INC_APP_APP_HPP_ */
