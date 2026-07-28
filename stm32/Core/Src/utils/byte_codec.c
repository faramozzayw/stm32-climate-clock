#include "utils/byte_codec.h"

void write_int16_le(uint8_t *destination, int16_t value)
{
	uint16_t encoded = (uint16_t)value;
	destination[0] = (uint8_t)(encoded & 0xFFU);
	destination[1] = (uint8_t)(encoded >> 8U);
}

uint16_t read_uint16_le(const uint8_t *source)
{
	return (uint16_t)source[0] |
		   ((uint16_t)source[1] << 8U);
}

int16_t read_int16_le(const uint8_t *source)
{
	const uint16_t encoded = read_uint16_le(source);
	const int32_t decoded = encoded > INT16_MAX
								? (int32_t)encoded - 65536
								: encoded;
	return (int16_t)decoded;
}

int16_t decode_int12(uint16_t encoded)
{
	const uint16_t value = encoded & 0x0FFFU;
	const int32_t decoded = (value & 0x0800U) != 0U
								? (int32_t)value - 4096
								: value;
	return (int16_t)decoded;
}

uint32_t read_uint20_be(const uint8_t *source)
{
	return ((uint32_t)source[0] << 12U) |
		   ((uint32_t)source[1] << 4U) |
		   ((uint32_t)source[2] >> 4U);
}
