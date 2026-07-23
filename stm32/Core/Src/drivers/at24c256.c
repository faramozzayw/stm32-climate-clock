/*
 * at24c256.c
 *
 *  Created on: Jul 21, 2026
 *      Author: faram
 */

#include <drivers/at24c256.h>

#define AT24C256_READY_TRIALS 1U
#define AT24C256_READY_POLL_INTERVAL_MS 1U

static AT24C256_Status status_from_hal(HAL_StatusTypeDef status)
{
	switch (status)
	{
	case HAL_OK:
		return AT24C256_OK;
	case HAL_BUSY:
		return AT24C256_BUSY;
	case HAL_TIMEOUT:
		return AT24C256_TIMEOUT;
	case HAL_ERROR:
	default:
		return AT24C256_ERROR;
	}
}

static uint16_t device_address(const at24c256_t *eeprom)
{
	return (uint16_t)(eeprom->address << 1U);
}

static AT24C256_Status validate_request(
	const at24c256_t *eeprom,
	uint16_t memory_address,
	const void *data,
	size_t length)
{
	if ((eeprom == NULL) || (eeprom->i2c == NULL) ||
		((length > 0U) && (data == NULL)))
	{
		return AT24C256_INVALID_ARGUMENT;
	}

	if (((size_t)memory_address + length) > AT24C256_SIZE_BYTES)
	{
		return AT24C256_OUT_OF_RANGE;
	}

	return AT24C256_OK;
}

AT24C256_Status at24c256_init(at24c256_t *eeprom, I2C_HandleTypeDef *i2c, uint8_t address, uint32_t timeout_ms)
{
	if ((eeprom == NULL) || (i2c == NULL) || (address > 0x7FU))
	{
		return AT24C256_INVALID_ARGUMENT;
	}

	eeprom->i2c = i2c;
	eeprom->address = address;
	eeprom->timeout_ms = timeout_ms;

	return at24c256_is_ready(eeprom);
}

AT24C256_Status at24c256_is_ready(const at24c256_t *eeprom)
{
	uint32_t start;

	if ((eeprom == NULL) || (eeprom->i2c == NULL))
	{
		return AT24C256_INVALID_ARGUMENT;
	}

	start = HAL_GetTick();

	while (1)
	{
		if (HAL_I2C_IsDeviceReady(eeprom->i2c,
				device_address(eeprom),
				AT24C256_READY_TRIALS,
				eeprom->timeout_ms) == HAL_OK)
		{
			return AT24C256_OK;
		}

		if ((HAL_GetTick() - start) >= eeprom->timeout_ms)
		{
			return AT24C256_TIMEOUT;
		}

		HAL_Delay(AT24C256_READY_POLL_INTERVAL_MS);
	}
}

AT24C256_Status at24c256_read(const at24c256_t *eeprom, uint16_t memory_address, uint8_t *data, size_t length)
{
	AT24C256_Status status = validate_request(eeprom, memory_address,
		data, length);
	if (status != AT24C256_OK)
	{
		return status;
	}

	while (length > 0U)
	{
		uint16_t chunk = (length > UINT16_MAX) ? UINT16_MAX : (uint16_t)length;

		status = status_from_hal(HAL_I2C_Mem_Read(eeprom->i2c,
			device_address(eeprom),
			memory_address,
			I2C_MEMADD_SIZE_16BIT,
			data,
			chunk,
			eeprom->timeout_ms));
		if (status != AT24C256_OK)
		{
			return status;
		}

		memory_address = (uint16_t)(memory_address + chunk);
		data += chunk;
		length -= chunk;
	}

	return AT24C256_OK;
}

AT24C256_Status at24c256_write(const at24c256_t *eeprom, uint16_t memory_address, const uint8_t *data, size_t length)
{
	AT24C256_Status status = validate_request(eeprom, memory_address,
		data, length);
	if (status != AT24C256_OK)
	{
		return status;
	}

	while (length > 0U)
	{
		size_t page_remaining = AT24C256_PAGE_SIZE -
								(memory_address % AT24C256_PAGE_SIZE);
		uint16_t chunk = (uint16_t)((length < page_remaining)
										? length
										: page_remaining);

		status = status_from_hal(HAL_I2C_Mem_Write(eeprom->i2c,
			device_address(eeprom),
			memory_address,
			I2C_MEMADD_SIZE_16BIT,
			(uint8_t *)data,
			chunk,
			eeprom->timeout_ms));
		if (status != AT24C256_OK)
		{
			return status;
		}

		status = at24c256_is_ready(eeprom);
		if (status != AT24C256_OK)
		{
			return status;
		}

		memory_address = (uint16_t)(memory_address + chunk);
		data += chunk;
		length -= chunk;
	}

	return AT24C256_OK;
}

AT24C256_Status at24c256_read_byte(const at24c256_t *eeprom, uint16_t memory_address, uint8_t *value)
{
	return at24c256_read(eeprom, memory_address, value, 1U);
}

AT24C256_Status at24c256_write_byte(const at24c256_t *eeprom, uint16_t memory_address, uint8_t value)
{
	return at24c256_write(eeprom, memory_address, &value, 1U);
}
