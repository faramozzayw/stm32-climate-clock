/*
 * ds18B20.h
 *
 *  Created on: Jun 27, 2026
 *      Author: faram
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include <stdint.h>
#include "tim.h"

typedef struct
{
	TIM_HandleTypeDef *htim;
	uint32_t pin;
	GPIO_TypeDef *port;
} ds18b20_t;


void ds18b20_init(ds18b20_t *ds18b20);
uint8_t ds18b20_start(ds18b20_t *ds18b20);
void ds18b20_write(ds18b20_t *ds18b20, uint8_t data);
uint8_t ds18b20_read(ds18b20_t *ds18b20);
float ds18b29_get_temp(ds18b20_t *ds18b20);

#endif /* INC_DS18B20_H_ */
