#include "temperature_indicator.h"

void temperature_indicator_init(hw479_t *indicator)
{
	if (indicator == NULL)
	{
		return;
	}

	hw479_init(indicator);
}

void temperature_indicator_update(hw479_t *indicator, int16_t temperature, int16_t min_temperature, int16_t max_temperature)
{
	if (indicator == NULL)
	{
		return;
	}

	if (temperature >= max_temperature)
	{
		hw479_set_colors(indicator, 999, 0, 0);
	}
	else if (temperature <= min_temperature)
	{
		hw479_set_colors(indicator, 0, 0, 999);
	}
	else
	{
		hw479_set_colors(indicator, 0, 0, 0);
	}
}
