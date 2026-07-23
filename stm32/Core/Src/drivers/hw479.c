/*
 * hw479.c
 *
 *  Created on: Jun 24, 2026
 *      Author: faram
 */

#include "drivers/hw479.h"

void hw479_set_colors(hw479_t *hw479, uint32_t red, uint32_t green, uint32_t blue)
{
	__HAL_TIM_SET_COMPARE(hw479->htim, hw479->red_ch, red);
	__HAL_TIM_SET_COMPARE(hw479->htim, hw479->green_ch, green);
	__HAL_TIM_SET_COMPARE(hw479->htim, hw479->blue_ch, blue);
}

void hw479_reset(hw479_t *hw479)
{
	hw479_set_colors(hw479, 0, 0, 0);
}

void hw479_init(hw479_t *hw479)
{
	HAL_TIM_PWM_Start(hw479->htim, hw479->red_ch);
	HAL_TIM_PWM_Start(hw479->htim, hw479->green_ch);
	HAL_TIM_PWM_Start(hw479->htim, hw479->blue_ch);

	hw479_reset(hw479);
}
