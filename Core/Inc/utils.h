/*
 * utils.h
 *
 *  Created on: Jun 27, 2026
 *      Author: faram
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "tim.h"

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void delay(TIM_HandleTypeDef *htim, uint16_t us);
int16_t tempToFixed(float temp);

#endif /* INC_UTILS_H_ */
