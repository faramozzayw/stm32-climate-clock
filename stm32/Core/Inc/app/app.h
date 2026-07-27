#ifndef INC_APP_APP_H_
#define INC_APP_APP_H_

#include <stdbool.h>
#include <stdint.h>

#include "command_receiver/uart_command_receiver.h"
#include "drivers/at24c256.h"
#include "drivers/ds3231.h"
#include "drivers/hw479.h"
#include "drivers/lcd1602.h"
#include "platform/hal_uart_transport.h"
#include "utils/temperature.h"

#define APP_UPDATE_INTERVAL_MS 250U

typedef struct
{
	lcd1602_t *lcd;
	hw479_t *hw479;
	ds3231_t *rtc;
	at24c256_t *eeprom;
	uart_command_receiver_t *command_receiver;
	hal_uart_transport_t *telemetry_transport;
	int16_t min_temp;
	int16_t max_temp;
	temperature_unit_t temperature_unit;
	bool eeprom_ready;
	ble_connection_state_t ble_connection_state;
} app_t;

/**
 * @brief Initialize application devices and load persisted settings.
 *
 * @param app Application state to initialize.
 * @param lcd Configured LCD device.
 * @param hw479 Configured RGB indicator device.
 * @param rtc Configured real-time clock.
 * @param eeprom EEPROM state to initialize.
 * @param eeprom_i2c I2C peripheral used by the EEPROM.
 * @param command_receiver UART command receiver state.
 * @param telemetry_transport UART transport used to send telemetry.
 * @return true when required application components initialized successfully.
 *         EEPROM availability is optional and is tracked in the application.
 */
bool app_init(app_t *app,
	lcd1602_t *lcd,
	hw479_t *hw479,
	ds3231_t *rtc,
	at24c256_t *eeprom,
	I2C_HandleTypeDef *eeprom_i2c,
	uart_command_receiver_t *command_receiver,
	hal_uart_transport_t *telemetry_transport);

/**
 * @brief Process received commands and apply their requested changes.
 *
 * @param app Initialized application state.
 */
void app_poll(app_t *app);

/**
 * @brief Refresh the clock, temperature display, and temperature indicator.
 *
 * @param app Initialized application state.
 */
void app_update(app_t *app);

/**
 * @brief Select the unit used for temperature display.
 *
 * Stored temperatures and thresholds remain in Celsius.
 *
 * @param app Initialized application state.
 * @param unit Celsius or Fahrenheit display unit.
 */
void app_set_temperature_unit(app_t *app, temperature_unit_t unit);

#endif /* INC_APP_APP_H_ */
