#ifndef SRC_APP_TELEMETRY_H_
#define SRC_APP_TELEMETRY_H_

#include <stdbool.h>
#include <stdint.h>

#include "stm32l0xx_hal.h"

/**
 * @brief Encode and send current temperature settings as DeviceTelemetry.
 *
 * @param uart UART connected to the ESP32.
 * @param current_temperature Current temperature in tenths of a degree Celsius.
 * @param min_temperature Configured minimum in tenths of a degree Celsius.
 * @param max_temperature Configured maximum in tenths of a degree Celsius.
 * @return true when the complete framed message was transmitted.
 */
bool telemetry_send_temperature(
	UART_HandleTypeDef *uart,
	int16_t current_temperature,
	int16_t min_temperature,
	int16_t max_temperature);

#endif /* SRC_APP_TELEMETRY_H_ */
