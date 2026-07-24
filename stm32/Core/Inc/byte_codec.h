#ifndef INC_BYTE_CODEC_H_
#define INC_BYTE_CODEC_H_

#include <stdint.h>

void write_int16_le(uint8_t *destination, int16_t value);
int16_t read_int16_le(const uint8_t *source);

#endif /* INC_BYTE_CODEC_H_ */
