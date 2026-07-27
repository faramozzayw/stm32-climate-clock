#ifndef TEST_FAKE_STM32L0XX_HAL_H_
#define TEST_FAKE_STM32L0XX_HAL_H_

#include <stdint.h>

#define HAL_MAX_DELAY UINT32_MAX

typedef struct
{
	uint32_t instance;
} I2C_HandleTypeDef;

typedef enum
{
	HAL_OK = 0x00U,
	HAL_ERROR = 0x01U,
} HAL_StatusTypeDef;

HAL_StatusTypeDef HAL_I2C_Master_Transmit(
	I2C_HandleTypeDef *i2c,
	uint16_t address,
	uint8_t *data,
	uint16_t size,
	uint32_t timeout);
void HAL_Delay(uint32_t delay_ms);

#endif /* TEST_FAKE_STM32L0XX_HAL_H_ */
