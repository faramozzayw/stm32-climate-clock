#include "utils/temperature.h"

#include <stdio.h>

int16_t centi_celsius_to_tenths(int32_t centi_celsius)
{
	return (int16_t)((centi_celsius >= 0
							 ? centi_celsius + 5
							 : centi_celsius - 5) /
					 10);
}

int16_t celsius_to_fahrenheit_fixed(int16_t celsius_tenths)
{
	int32_t scaled = (int32_t)celsius_tenths * 9;
	return (int16_t)((scaled >= 0 ? scaled + 2 : scaled - 2) / 5 + 320);
}

bool fixed_temperature_to_string(
	char *buffer,
	size_t buffer_size,
	int16_t temperature_tenths,
	char unit)
{
	int32_t magnitude;
	int written;

	if ((buffer == NULL) || (buffer_size == 0U))
	{
		return false;
	}

	magnitude = temperature_tenths;
	if (magnitude < 0)
	{
		magnitude = -magnitude;
	}

	written = snprintf(
		buffer,
		buffer_size,
		"%s%ld.%ld %c",
		temperature_tenths < 0 ? "-" : "",
		(long)(magnitude / 10),
		(long)(magnitude % 10),
		unit);

	return (written >= 0) && ((size_t)written < buffer_size);
}
