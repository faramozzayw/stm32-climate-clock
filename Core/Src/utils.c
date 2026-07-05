/*
 * utils.c
 *
 *  Created on: Jun 27, 2026
 *      Author: faram
 */

#include "utils.h"
#include <math.h>

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void delay(TIM_HandleTypeDef *htim, uint16_t us)
{
	__HAL_TIM_SET_COUNTER(htim, 0); // set the counter value a 0
	while (__HAL_TIM_GET_COUNTER(htim) < us); // wait for the counter to reach the us input in the parameter
}

int16_t tempToFixed(float temp)
{
    return (int16_t)lroundf(temp * 10.0f);
}
