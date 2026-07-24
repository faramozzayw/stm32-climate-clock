/*
 * ds18b20.c
 *
 *  Created on: Jun 27, 2026
 *      Author: faram
 */

#include "drivers/ds18b20.h"
#include "platform/hal_gpio_timer.h"

void ds18b20_init(ds18b20_t *ds18b20)
{
	HAL_TIM_Base_Start(ds18b20->htim);
}

uint8_t ds18b20_start(ds18b20_t *ds18b20)
{
	uint8_t Response = 0;
	Set_Pin_Output(ds18b20->port, ds18b20->pin);	   // set the pin as output
	HAL_GPIO_WritePin(ds18b20->port, ds18b20->pin, 0); // pull the pin low
	delay(ds18b20->htim, 480);						   // delay according to datasheet

	Set_Pin_Input(ds18b20->port, ds18b20->pin); // set the pin as input

	delay(ds18b20->htim, 80); // delay according to datasheet
	if (!(HAL_GPIO_ReadPin(ds18b20->port, ds18b20->pin)))
		Response = 1; // if the pin is low i.e the presence pulse is detected
	else
		Response = -1;

	delay(ds18b20->htim, 400); // 480 us delay totally.

	return Response;
}

void ds18b20_write(ds18b20_t *ds18b20, uint8_t data)
{
	Set_Pin_Output(ds18b20->port, ds18b20->pin); // set as output

	for (int i = 0; i < 8; i++)
	{
		if ((data & (1 << i)) != 0) // if the bit is high
		{
			// write 1
			Set_Pin_Output(ds18b20->port, ds18b20->pin);	   // set as output
			HAL_GPIO_WritePin(ds18b20->port, ds18b20->pin, 0); // pull the pin LOW
			delay(ds18b20->htim, 1);						   // wait for 1 us

			Set_Pin_Input(ds18b20->port, ds18b20->pin); // set as input
			delay(ds18b20->htim, 50);					// wait for 60 us
		}
		else // if the bit is low
		{
			// write 0
			Set_Pin_Output(ds18b20->port, ds18b20->pin);
			HAL_GPIO_WritePin(ds18b20->port, ds18b20->pin, 0); // pull the pin LOW
			delay(ds18b20->htim, 50);						   // wait for 60 us

			Set_Pin_Input(ds18b20->port, ds18b20->pin);
		}
	}
}

uint8_t ds18b20_read(ds18b20_t *ds18b20)
{
	uint8_t value = 0;
	Set_Pin_Input(ds18b20->port, ds18b20->pin);

	for (int i = 0; i < 8; i++)
	{
		Set_Pin_Output(ds18b20->port, ds18b20->pin);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, 0); // pull the data pin LOW
		delay(ds18b20->htim, 2);				 // wait for 2 us
		Set_Pin_Input(ds18b20->port, ds18b20->pin);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)) // if the pin is HIGH
		{
			value |= 1 << i; // read = 1
		}
		delay(ds18b20->htim, 60); // wait for 60 us
	}
	return value;
}

float ds18b29_get_temp(ds18b20_t *ds18b20)
{
	ds18b20_start(ds18b20);
	ds18b20_write(ds18b20, 0xCC); // skip ROM
	ds18b20_write(ds18b20, 0x44); // convert t

	ds18b20_start(ds18b20);
	ds18b20_write(ds18b20, 0xCC); // skip ROM
	ds18b20_write(ds18b20, 0xBE); // Read Scratch-pad

	uint8_t temp_byte1 = ds18b20_read(ds18b20);
	uint8_t temp_byte2 = ds18b20_read(ds18b20);
	uint8_t temp = ((temp_byte2 << 8)) | temp_byte1;
	return (float)temp / 16.0; // resolution is 0.0625
}
