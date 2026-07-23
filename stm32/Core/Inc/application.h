#ifndef INC_APPLICATION_H_
#define INC_APPLICATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "command_receiver/uart_command_receiver.h"
#include "drivers/at24c256.h"
#include "drivers/ds3231.h"
#include "drivers/hw479.h"
#include "drivers/lcd1602.h"

#define APPLICATION_UPDATE_INTERVAL_MS 750U

typedef struct
{
	lcd1602_t *lcd;
	hw479_t *hw479;
	ds3231_t *rtc;
	at24c256_t *eeprom;
	uart_command_receiver_t *command_receiver;
	UART_HandleTypeDef *command_uart;
	int16_t min_temp;
	int16_t max_temp;
	bool eeprom_ready;
} application_t;

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
 * @param command_uart UART peripheral used to receive commands.
 * @return true when required application components initialized successfully.
 *         EEPROM availability is optional and is tracked in the application.
 */
bool application_init(application_t *app,
					  lcd1602_t *lcd,
					  hw479_t *hw479,
					  ds3231_t *rtc,
					  at24c256_t *eeprom,
					  I2C_HandleTypeDef *eeprom_i2c,
					  uart_command_receiver_t *command_receiver,
					  UART_HandleTypeDef *command_uart);

/**
 * @brief Process received commands and apply their requested changes.
 *
 * @param app Initialized application state.
 */
void application_poll(application_t *app);

/**
 * @brief Refresh the clock, temperature display, and temperature indicator.
 *
 * @param app Initialized application state.
 */
void application_update(application_t *app);

/**
 * @brief Forward a completed UART receive interrupt to the application.
 *
 * Interrupts from UART peripherals other than the configured command UART are
 * ignored.
 *
 * @param app Initialized application state.
 * @param uart UART that completed a receive operation.
 */
void application_on_uart_rx_complete(application_t *app,
									 UART_HandleTypeDef *uart);

#endif /* INC_APPLICATION_H_ */
