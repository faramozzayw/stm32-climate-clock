/*
 * utils.h
 *
 *  Created on: Jun 27, 2026
 *      Author: faram
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/ds3231.h"
#include "tim.h"

typedef enum
{
	TEMPERATURE_UNIT_CELSIUS = 0,
	TEMPERATURE_UNIT_FAHRENHEIT
} temperature_unit_t;

#define TEMPERATURE_UNIT_CELSIUS_SYMBOL 'C'
#define TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL 'F'

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void delay(TIM_HandleTypeDef *htim, uint16_t us);
int16_t tempToFixed(float temp);
/**
 * @brief Convert tenths of a degree Celsius to tenths of a degree Fahrenheit.
 */
int16_t celsius_to_fahrenheit_fixed(int16_t celsius_tenths);

/**
 * @brief Format a signed fixed-point temperature with one decimal place.
 *
 * @param buffer Destination for the null-terminated string.
 * @param buffer_size Size of the destination buffer.
 * @param temperature_tenths Temperature in tenths of a degree.
 * @param unit Unit symbol appended to the string.
 * @return true when the complete string was written; otherwise false.
 */
bool fixed_temperature_to_string(
	char *buffer,
	size_t buffer_size,
	int16_t temperature_tenths,
	char unit);
void write_int16_le(uint8_t *destination, int16_t value);
int16_t read_int16_le(const uint8_t *source);
bool epoch_ms_to_ds3231_time(uint64_t epoch_ms, ds3231_time_t *time);

#endif /* INC_UTILS_H_ */
