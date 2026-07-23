#ifndef SRC_APP_TELEMETRY_H_
#define SRC_APP_TELEMETRY_H_

#include <stdbool.h>
#include <stdint.h>

#include "stm32l0xx_hal.h"

/**
 * @brief Encode and send the current temperature as DeviceTelemetry.
 *
 * @param uart UART connected to the ESP32.
 * @param temperature_celsius Temperature in tenths of a degree Celsius.
 * @return true when the complete framed message was transmitted.
 */
bool telemetry_send_temperature(
	UART_HandleTypeDef *uart,
	int16_t temperature_celsius);

#endif /* SRC_APP_TELEMETRY_H_ */
