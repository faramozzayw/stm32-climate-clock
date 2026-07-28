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

void ds3231_set_time(ds3231_t *ds3231, calendar_time_t time)
{
	uint8_t set_time[7];
	set_time[0] = decToBcd(time.second);
	set_time[1] = decToBcd(time.minute);
	set_time[2] = decToBcd(time.hour);
	set_time[3] = decToBcd(time.weekday);
	set_time[4] = decToBcd(time.day);
	set_time[5] = decToBcd(time.month);
	set_time[6] = decToBcd(time.year - 2000U);

	HAL_I2C_Mem_Write(ds3231->i2c, ds3231->addr, 0x00, 1, set_time, 7, 1000);
}

calendar_time_t ds3231_get_time(ds3231_t *ds3231)
{
	calendar_time_t time;

	uint8_t get_time[7];
	HAL_I2C_Mem_Read(ds3231->i2c, ds3231->addr, 0x00, 1, get_time, 7, 1000);
	time.second = bcdToDec(get_time[0]);
	time.minute = bcdToDec(get_time[1]);
	time.hour = bcdToDec(get_time[2]);
	time.weekday = bcdToDec(get_time[3]);
	time.day = bcdToDec(get_time[4]);
	time.month = bcdToDec(get_time[5]);
	time.year = (uint16_t)(2000U + bcdToDec(get_time[6]));
	return time;
}
