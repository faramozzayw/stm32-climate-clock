#include "app/app.h"

#include <stdio.h>

#include "screen.h"
#include "temperature_indicator.h"
#include "temperature_settings.h"
#include "utils.h"

#define DEFAULT_MIN_TEMP 100
#define DEFAULT_MAX_TEMP 300

/**
 * @brief Apply decoded UART command values to application state and devices.
 *
 * Temperature changes are persisted when EEPROM is available. Current-time
 * commands are converted from Unix epoch milliseconds before updating the RTC.
 *
 * @param app Initialized application state.
 */
static void apply_uart_commands(app_t *app)
{
	uart_command_receiver_values_t *values = &app->command_receiver->values;

	if (values->min_temp_updated)
	{
		app->min_temp = values->min_temp;
		values->min_temp_updated = false;

		if (app->eeprom_ready &&
			save_min_temperature(app->eeprom, app->min_temp) != AT24C256_OK)
		{
			printf("Failed to save minimum temperature\r\n");
		}
	}

	if (values->max_temp_updated)
	{
		app->max_temp = values->max_temp;
		values->max_temp_updated = false;

		if (app->eeprom_ready &&
			save_max_temperature(app->eeprom, app->max_temp) != AT24C256_OK)
		{
			printf("Failed to save maximum temperature\r\n");
		}
	}

	if (values->current_time_updated)
	{
		ds3231_time_t time;

		if (epoch_ms_to_ds3231_time(values->current_time_ms, &time))
		{
			ds3231_set_time(app->rtc, time);
			printf("RTC set to %02u:%02u:%02u %02u/%02u/20%02u UTC\r\n",
				(unsigned int)time.hour,
				(unsigned int)time.minutes,
				(unsigned int)time.seconds,
				(unsigned int)time.dayofmonth,
				(unsigned int)time.month,
				(unsigned int)time.year);
		}
		else
		{
			printf("SetCurrentTime is outside DS3231 range (2000-2099)\r\n");
		}

		values->current_time_updated = false;
	}
}

bool app_init(
	app_t *app,
	lcd1602_t *lcd,
	hw479_t *hw479,
	ds3231_t *rtc,
	at24c256_t *eeprom,
	I2C_HandleTypeDef *eeprom_i2c,
	uart_command_receiver_t *command_receiver,
	UART_HandleTypeDef *command_uart)
{
	AT24C256_Status eeprom_status;

	if ((app == NULL) || (lcd == NULL) || (hw479 == NULL) ||
		(rtc == NULL) || (eeprom == NULL) || (eeprom_i2c == NULL) ||
		(command_receiver == NULL) || (command_uart == NULL))
	{
		return false;
	}

	app->lcd = lcd;
	app->hw479 = hw479;
	app->rtc = rtc;
	app->eeprom = eeprom;
	app->command_receiver = command_receiver;
	app->command_uart = command_uart;
	app->min_temp = DEFAULT_MIN_TEMP;
	app->max_temp = DEFAULT_MAX_TEMP;
	app->temperature_unit = TEMPERATURE_UNIT_CELSIUS;
	app->eeprom_ready = false;

	screen_init(app->lcd);
	temperature_indicator_init(app->hw479);

	eeprom_status = at24c256_init(
		app->eeprom,
		eeprom_i2c,
		AT24C256_DEFAULT_ADDRESS,
		100U);

	if (eeprom_status == AT24C256_OK)
	{
		app->eeprom_ready = true;

		if (load_temperature_settings(
				app->eeprom,
				&app->min_temp,
				&app->max_temp))
		{
			printf("Loaded temperature settings: min=%d, max=%d\r\n",
				(int)app->min_temp,
				(int)app->max_temp);
		}
		else
		{
			printf("No valid temperature settings; using defaults: min=%d, max=%d\r\n",
				(int)app->min_temp,
				(int)app->max_temp);
		}
	}
	else
	{
		printf("AT24C256 unavailable; using default temperature settings\r\n");
	}

	app->command_receiver->values.min_temp = app->min_temp;
	app->command_receiver->values.max_temp = app->max_temp;
	uart_command_receiver_init(app->command_receiver, app->command_uart);

	if (HAL_UART_Receive_IT(
			app->command_uart,
			uart_command_receiver_rx_byte_ptr(app->command_receiver),
			1U) != HAL_OK)
	{
		printf("Command UART RX IT start failed\r\n");
		return false;
	}

	printf("Command UART RX IT started\r\n");
	return true;
}

void app_poll(app_t *app)
{
	if ((app == NULL) || (app->command_receiver == NULL))
	{
		return;
	}

	uart_command_receiver_poll(app->command_receiver);
	apply_uart_commands(app);
}

void app_update(app_t *app)
{
	ds3231_time_t time;
	int16_t temperature;

	if ((app == NULL) || (app->lcd == NULL) ||
		(app->hw479 == NULL) || (app->rtc == NULL))
	{
		return;
	}

	time = ds3231_get_time(app->rtc);
	ds3231_force_temp_conv(app->rtc);
	temperature = ds3231_get_temp_fixed(app->rtc);

	screen_update(app->lcd, &time, temperature, app->temperature_unit);
	temperature_indicator_update(app->hw479, temperature, app->min_temp, app->max_temp);
}

void app_set_temperature_unit(app_t *app, temperature_unit_t unit)
{
	if ((app == NULL) ||
		((unit != TEMPERATURE_UNIT_CELSIUS) &&
			(unit != TEMPERATURE_UNIT_FAHRENHEIT)))
	{
		return;
	}

	app->temperature_unit = unit;
}

void app_on_uart_rx_complete(app_t *app, UART_HandleTypeDef *uart)
{
	if ((app == NULL) || (uart != app->command_uart) ||
		(app->command_receiver == NULL))
	{
		return;
	}

	uart_command_receiver_on_rx_complete(app->command_receiver);
}
