#ifndef INC_PLATFORM_HAL_GPIO_TIMER_H_
#define INC_PLATFORM_HAL_GPIO_TIMER_H_

#include "tim.h"

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void delay(TIM_HandleTypeDef *htim, uint16_t us);

#endif /* INC_PLATFORM_HAL_GPIO_TIMER_H_ */
