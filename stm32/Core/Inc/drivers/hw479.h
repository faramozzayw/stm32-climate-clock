/*
 * hw479.h
 *
 *  Created on: Jun 24, 2026
 *      Author: faram
 */

#ifndef INC_HW479_H_
#define INC_HW479_H_

#include "tim.h"

typedef struct
{
	TIM_HandleTypeDef *htim;
	uint32_t red_ch;
	uint32_t green_ch;
	uint32_t blue_ch;
} hw479_t;

#ifdef __cplusplus
extern "C"
{
#endif

	void hw479_init(hw479_t *hw479);
	void hw479_reset(hw479_t *hw479);
	void hw479_set_colors(hw479_t *hw479, uint32_t red, uint32_t green, uint32_t blue);
	TIM_HandleTypeDef *hw479_get_timer(const hw479_t *hw479);

#ifdef __cplusplus
}
#endif

#endif /* INC_HW479_H_ */
