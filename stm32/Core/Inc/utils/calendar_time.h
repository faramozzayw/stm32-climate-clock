#ifndef INC_UTILS_CALENDAR_TIME_H_
#define INC_UTILS_CALENDAR_TIME_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t weekday;
	uint8_t day;
	uint8_t month;
	uint16_t year;
} calendar_time_t;

/**
 * @brief Convert Unix epoch milliseconds to a UTC calendar time.
 *
 * The supported calendar range is 2000-01-01 through 2099-12-31. Weekdays use
 * 1 for Sunday through 7 for Saturday.
 *
 * @param epoch_ms Unix epoch time in milliseconds.
 * @param time Destination calendar value.
 * @return true when the input is in the supported range; otherwise false.
 */
bool calendar_time_from_unix_ms(uint64_t epoch_ms, calendar_time_t *time);

#endif /* INC_UTILS_CALENDAR_TIME_H_ */
