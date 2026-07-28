#ifndef INC_APP_SCREEN_HPP_
#define INC_APP_SCREEN_HPP_

#include <cstdint>

#include "drivers/lcd1602.h"
#include "utils/calendar_time.h"
#include "utils/temperature.h"

namespace climate_clock
{
class Screen
{
  public:
	explicit Screen(lcd1602_t &lcd);

	void initialize();
	void update(
		const calendar_time_t &time,
		std::int16_t temperature_celsius,
		temperature_unit_t unit);

  private:
	lcd1602_t &lcd_;
};
} // namespace climate_clock

#endif /* INC_APP_SCREEN_HPP_ */
