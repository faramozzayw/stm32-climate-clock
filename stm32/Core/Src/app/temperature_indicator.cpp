#include "app/temperature_indicator.hpp"

namespace climate_clock
{
TemperatureIndicator::TemperatureIndicator(hw479_t &driver)
	: driver_(driver)
{
}

void TemperatureIndicator::initialize()
{
	hw479_init(&driver_);
}

void TemperatureIndicator::update(
	std::int16_t temperature,
	std::int16_t min_temperature,
	std::int16_t max_temperature)
{
	if (temperature >= max_temperature)
	{
		hw479_set_colors(&driver_, 999U, 0U, 0U);
	}
	else if (temperature <= min_temperature)
	{
		hw479_set_colors(&driver_, 0U, 0U, 999U);
	}
	else
	{
		hw479_set_colors(&driver_, 0U, 0U, 0U);
	}
}
} // namespace climate_clock
