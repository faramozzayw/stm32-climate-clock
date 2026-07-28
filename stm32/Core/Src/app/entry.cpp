#include "app/entry.h"

#include <cstdint>
#include <cstdio>

#include "app/app.hpp"
#include "command_receiver/uart_command_receiver.h"
#include "drivers/at24c256.h"
#include "drivers/bmx280.hpp"
#include "drivers/ds3231.h"
#include "drivers/hw479.h"
#include "drivers/lcd1602.h"
#include "i2c.h"
#include "main.h"
#include "platform/hal_uart_receiver.h"
#include "tim.h"
#include "usart.h"

namespace
{
uart_command_receiver_t command_receiver{};
hal_uart_receiver_t command_uart_receiver{};
} // namespace

void app_main(void)
{
	hal_uart_transport_t telemetry_transport{&huart1, 100U};
	lcd1602_t lcd{&hi2c1, 0x27U << 1U};
	hw479_t temperature_indicator{
		&htim2,
		TIM_CHANNEL_1,
		TIM_CHANNEL_2,
		TIM_CHANNEL_3};
	at24c256_t eeprom{};
	ds3231_t rtc{&hi2c1, 0xd0U};
	climate_clock::Bmx280 environment_sensor{hi2c1};

	climate_clock::App app{
		lcd,
		temperature_indicator,
		rtc,
		environment_sensor,
		eeprom,
		hi2c1,
		command_receiver,
		telemetry_transport};

	if (!app.initialize())
	{
		Error_Handler();
	}

	if (!hal_uart_receiver_init(
			&command_uart_receiver,
			&huart1,
			&command_receiver) ||
		!hal_uart_receiver_start(&command_uart_receiver))
	{
		std::printf("Command UART RX IT start failed\r\n");
		Error_Handler();
	}

	std::printf("Command UART RX IT started\r\n");

	HAL_Delay(1000U);
	std::printf("-------------------------\r\n");

	auto last_update = HAL_GetTick();

	while (true)
	{
		app.poll();
		hal_uart_receiver_report_errors(&command_uart_receiver);

		if ((HAL_GetTick() - last_update) >=
			climate_clock::App::update_interval_ms)
		{
			last_update += climate_clock::App::update_interval_ms;
			app.update();
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart)
{
	hal_uart_receiver_on_complete(&command_uart_receiver, uart);
}
