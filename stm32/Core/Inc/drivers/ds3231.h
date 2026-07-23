/*
 * ds3231.h
 *
 *  Created on: Jun 25, 2026
 *      Author: faram
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "i2c.h"

typedef struct
{
	I2C_HandleTypeDef *i2c;
	uint8_t addr;
} ds3231_t;

typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
} ds3231_time_t;

void ds3231_set_time(ds3231_t *ds3231, ds3231_time_t time);
ds3231_time_t ds3231_get_time(ds3231_t *ds3231);
/**
 * @brief Read temperature in tenths of a degree Celsius.
 *
 * The DS3231 quarter-degree value is rounded to the nearest tenth, with
 * half-tenths rounded away from zero.
 */
int16_t ds3231_get_temp_fixed(ds3231_t *ds3231);
float ds3231_get_temp(ds3231_t *ds3231);
void ds3231_force_temp_conv(ds3231_t *ds3231);

#endif /* INC_DS3231_H_ */
