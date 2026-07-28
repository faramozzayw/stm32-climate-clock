/*
 * at24c256.h
 *
 *  Created on: Jul 21, 2026
 *      Author: faram
 */

#ifndef INC_DRIVERS_AT24C256_H_
#define INC_DRIVERS_AT24C256_H_

#include "stm32l0xx_hal.h"
#include <stdio.h>
#include <stdint.h>

#define AT24C256_SIZE_BYTES 32768U
#define AT24C256_PAGE_SIZE 64U
#define AT24C256_DEFAULT_ADDRESS 0x50U

typedef enum
{
	AT24C256_OK = 0,
	AT24C256_ERROR,
	AT24C256_BUSY,
	AT24C256_TIMEOUT,
	AT24C256_INVALID_ARGUMENT,
	AT24C256_OUT_OF_RANGE
} AT24C256_Status;

typedef struct
{
	I2C_HandleTypeDef *i2c;
	uint8_t address;
	uint32_t timeout_ms;
} at24c256_t;

#ifdef __cplusplus
extern "C"
{
#endif

	AT24C256_Status at24c256_init(at24c256_t *eeprom, I2C_HandleTypeDef *i2c, uint8_t address, uint32_t timeout_ms);

	AT24C256_Status at24c256_is_ready(const at24c256_t *eeprom);

	AT24C256_Status at24c256_read(const at24c256_t *eeprom, uint16_t memory_address, uint8_t *data, size_t length);

	AT24C256_Status at24c256_write(const at24c256_t *eeprom, uint16_t memory_address, const uint8_t *data, size_t length);

	AT24C256_Status at24c256_read_byte(const at24c256_t *eeprom, uint16_t memory_address, uint8_t *value);

	AT24C256_Status at24c256_write_byte(const at24c256_t *eeprom, uint16_t memory_address, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* INC_DRIVERS_AT24C256_H_ */
