#include "utils/calendar_time.h"

#include <stddef.h>

bool calendar_time_from_unix_ms(uint64_t epoch_ms, calendar_time_t *time)
{
	static const uint8_t days_per_month[12] = {
		31U, 28U, 31U, 30U, 31U, 30U,
		31U, 31U, 30U, 31U, 30U, 31U};
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
	if ((unix_seconds < unix_2000) || (unix_seconds >= unix_2100))
	{
		return false;
	}

	seconds_of_day = (uint32_t)(unix_seconds % 86400ULL);
	days = (uint32_t)((unix_seconds - unix_2000) / 86400ULL);

	while (true)
	{
		uint16_t days_in_year =
			((year % 4U) == 0U &&
				((year % 100U) != 0U || (year % 400U) == 0U))
				? 366U
				: 365U;

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

		if ((month == 2U) &&
			(year % 4U) == 0U &&
			((year % 100U) != 0U || (year % 400U) == 0U))
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

	time->second = (uint8_t)(seconds_of_day % 60U);
	time->minute = (uint8_t)((seconds_of_day / 60U) % 60U);
	time->hour = (uint8_t)(seconds_of_day / 3600U);
	time->weekday =
		(uint8_t)(((unix_seconds / 86400ULL + 4ULL) % 7ULL) + 1ULL);
	time->day = (uint8_t)(days + 1U);
	time->month = month;
	time->year = year;

	return true;
}
