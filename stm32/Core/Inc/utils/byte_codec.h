#ifndef INC_UTILS_BYTE_CODEC_H_
#define INC_UTILS_BYTE_CODEC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

	void write_int16_le(uint8_t *destination, int16_t value);
	uint16_t read_uint16_le(const uint8_t *source);
	int16_t read_int16_le(const uint8_t *source);
	int16_t decode_int12(uint16_t encoded);
	uint32_t read_uint20_be(const uint8_t *source);

#ifdef __cplusplus
}
#endif

#endif /* INC_UTILS_BYTE_CODEC_H_ */
