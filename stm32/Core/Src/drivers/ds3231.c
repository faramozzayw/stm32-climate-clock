/*
 * ds3231.c
 *
 *  Created on: Jun 25, 2026
 *      Author: faram
 */

#include "drivers/ds3231.h"

#define DS3231_ADDRESS 0xD0

// Convert normal decimal numbers to binary coded decimal
static uint8_t decToBcd(int val)
{
	return (uint8_t)((val / 10 * 16) + (val % 10));
}
// Convert binary coded decimal to normal decimal numbers
static int bcdToDec(uint8_t val)
{
	return (int)((val / 16 * 10) + (val % 16));
}

void ds3231_set_time(ds3231_t *ds3231, ds3231_time_t time)
{
	uint8_t set_time[7];
	set_time[0] = decToBcd(time.seconds);
	set_time[1] = decToBcd(time.minutes);
	set_time[2] = decToBcd(time.hour);
	set_time[3] = decToBcd(time.dayofweek);
	set_time[4] = decToBcd(time.dayofmonth);
	set_time[5] = decToBcd(time.month);
	set_time[6] = decToBcd(time.year);

	HAL_I2C_Mem_Write(ds3231->i2c, ds3231->addr, 0x00, 1, set_time, 7, 1000);
}

ds3231_time_t ds3231_get_time(ds3231_t *ds3231)
{
	ds3231_time_t time;

	uint8_t get_time[7];
	HAL_I2C_Mem_Read(ds3231->i2c, ds3231->addr, 0x00, 1, get_time, 7, 1000);
	time.seconds = bcdToDec(get_time[0]);
	time.minutes = bcdToDec(get_time[1]);
	time.hour = bcdToDec(get_time[2]);
	time.dayofweek = bcdToDec(get_time[3]);
	time.dayofmonth = bcdToDec(get_time[4]);
	time.month = bcdToDec(get_time[5]);
	time.year = bcdToDec(get_time[6]);
	return time;
}

int16_t ds3231_get_temp_fixed(ds3231_t *ds3231)
{
	uint8_t temp[2];
	int16_t quarter_degrees;
	int16_t tenths_x2;

	HAL_I2C_Mem_Read(ds3231->i2c, ds3231->addr, 0x11, 1, temp, 2, 1000);
	quarter_degrees = (int16_t)((int8_t)temp[0] * 4) +
					  (int16_t)(temp[1] >> 6);
	tenths_x2 = (int16_t)(quarter_degrees * 5);

	return tenths_x2 >= 0
			   ? (int16_t)((tenths_x2 + 1) / 2)
			   : (int16_t)((tenths_x2 - 1) / 2);
}

float ds3231_get_temp(ds3231_t *ds3231)
{
	return (float)ds3231_get_temp_fixed(ds3231) / 10.0f;
}

void ds3231_force_temp_conv(ds3231_t *ds3231)
{
	uint8_t status = 0;
	uint8_t control = 0;
	HAL_I2C_Mem_Read(ds3231->i2c, ds3231->addr, 0x0F, 1, &status, 1, 100); // read status register
	if (!(status & 0x04))
	{
		HAL_I2C_Mem_Read(ds3231->i2c, ds3231->addr, 0x0E, 1, &control, 1, 100); // read control register
		HAL_I2C_Mem_Write(ds3231->i2c, ds3231->addr, 0x0E, 1, (uint8_t *)(control | (0x20)), 1, 100);
	}
}
