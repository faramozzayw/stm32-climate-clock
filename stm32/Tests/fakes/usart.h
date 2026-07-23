#ifndef TEST_FAKE_USART_H_
#define TEST_FAKE_USART_H_

#include <stdint.h>

typedef struct
{
	uint32_t instance;
} UART_HandleTypeDef;

typedef enum
{
	HAL_OK = 0x00U,
	HAL_ERROR = 0x01U,
} HAL_StatusTypeDef;

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart,
		uint8_t *data, uint16_t size);

#endif /* TEST_FAKE_USART_H_ */
