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
#include "utils/calendar_time.h"
#include "i2c.h"

typedef struct
{
	I2C_HandleTypeDef *i2c;
	uint8_t addr;
} ds3231_t;

#ifdef __cplusplus
extern "C"
{
#endif

	void ds3231_set_time(ds3231_t *ds3231, calendar_time_t time);
	calendar_time_t ds3231_get_time(ds3231_t *ds3231);

#ifdef __cplusplus
}
#endif

#endif /* INC_DS3231_H_ */
