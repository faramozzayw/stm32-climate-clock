#include "byte_codec.h"

void write_int16_le(uint8_t *destination, int16_t value)
{
	uint16_t encoded = (uint16_t)value;
	destination[0] = (uint8_t)(encoded & 0xFFU);
	destination[1] = (uint8_t)(encoded >> 8U);
}

int16_t read_int16_le(const uint8_t *source)
{
	uint16_t encoded = (uint16_t)source[0] |
					   ((uint16_t)source[1] << 8U);
	return (int16_t)encoded;
}
