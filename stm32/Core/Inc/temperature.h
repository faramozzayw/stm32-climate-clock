#ifndef INC_TEMPERATURE_H_
#define INC_TEMPERATURE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum
{
	TEMPERATURE_UNIT_CELSIUS = 0,
	TEMPERATURE_UNIT_FAHRENHEIT
} temperature_unit_t;

#define TEMPERATURE_UNIT_CELSIUS_SYMBOL 'C'
#define TEMPERATURE_UNIT_FAHRENHEIT_SYMBOL 'F'

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

#endif /* INC_TEMPERATURE_H_ */
