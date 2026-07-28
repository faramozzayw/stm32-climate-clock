#ifndef INC_APP_TEMPERATURE_INDICATOR_HPP_
#define INC_APP_TEMPERATURE_INDICATOR_HPP_

#include <cstdint>

#include "drivers/hw479.h"

namespace climate_clock
{
class TemperatureIndicator
{
  public:
	explicit TemperatureIndicator(hw479_t &driver);

	void initialize();
	void update(
		std::int16_t temperature,
		std::int16_t min_temperature,
		std::int16_t max_temperature);

  private:
	hw479_t &driver_;
};
} // namespace climate_clock

#endif /* INC_APP_TEMPERATURE_INDICATOR_HPP_ */
