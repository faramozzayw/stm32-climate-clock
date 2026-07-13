/*
 * utils.h
 *
 *  Created on: Jun 27, 2026
 *      Author: faram
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include <stdbool.h>
#include <stdint.h>
#include "drivers/ds3231.h"
#include "tim.h"

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void delay(TIM_HandleTypeDef *htim, uint16_t us);
int16_t tempToFixed(float temp);
bool epoch_ms_to_ds3231_time(uint64_t epoch_ms, ds3231_time_t *time);

#endif /* INC_UTILS_H_ */
