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

bool epoch_ms_to_ds3231_time(uint64_t epoch_ms, ds3231_time_t *time)
{
	static const uint8_t days_per_month[12] =
	{
		31U, 28U, 31U, 30U, 31U, 30U,
		31U, 31U, 30U, 31U, 30U, 31U
	};
	const uint64_t unix_2000 = 946684800ULL;
	const uint64_t unix_2100 = 4102444800ULL;
	uint64_t unix_seconds;
	uint32_t days;
	uint32_t seconds_of_day;
	uint16_t year = 2000U;
	uint8_t month = 1U;

	if (time == NULL)
	{
		return false;
	}

	unix_seconds = epoch_ms / 1000ULL;
	if (unix_seconds < unix_2000 || unix_seconds >= unix_2100)
	{
		return false;
	}

	seconds_of_day = (uint32_t)(unix_seconds % 86400ULL);
	days = (uint32_t)((unix_seconds - unix_2000) / 86400ULL);

	while (true)
	{
		uint16_t days_in_year =
			((year % 4U) == 0U && ((year % 100U) != 0U || (year % 400U) == 0U))
			? 366U : 365U;

		if (days < days_in_year)
		{
			break;
		}

		days -= days_in_year;
		year++;
	}

	while (month <= 12U)
	{
		uint8_t days_in_month = days_per_month[month - 1U];
		if (month == 2U &&
			(year % 4U) == 0U && ((year % 100U) != 0U || (year % 400U) == 0U))
		{
			days_in_month = 29U;
		}

		if (days < days_in_month)
		{
			break;
		}

		days -= days_in_month;
		month++;
	}

	time->seconds = (uint8_t)(seconds_of_day % 60U);
	time->minutes = (uint8_t)((seconds_of_day / 60U) % 60U);
	time->hour = (uint8_t)(seconds_of_day / 3600U);
	/* DS3231 day-of-week uses 1..7. 1970-01-01 was a Thursday. */
	time->dayofweek = (uint8_t)(((unix_seconds / 86400ULL + 4ULL) % 7ULL) + 1ULL);
	time->dayofmonth = (uint8_t)(days + 1U);
	time->month = month;
	time->year = (uint8_t)(year - 2000U);

	return true;
}
