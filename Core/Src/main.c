/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "drivers/hw479.h"
#include "drivers/ds3231.h"
#include "drivers/ds18b20.h"
#include "drivers/lcd1602.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LCD_BACKLIGHT 0x08
#define UART_RX_LOG_SIZE 128U
#define UART_CMD_LINE_SIZE 64U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t rx_byte;
static uint8_t rx_log[UART_RX_LOG_SIZE];
static char uart_cmd_line[UART_CMD_LINE_SIZE];
static uint16_t uart_cmd_pos = 0;
static volatile uint16_t rx_log_head = 0;
static volatile uint16_t rx_log_tail = 0;
static volatile uint32_t rx_byte_count = 0;
static volatile uint32_t rx_error_count = 0;
static volatile uint32_t rx_overflow_count = 0;
static int16_t min_temp = 100; // 18.0°C
static int16_t max_temp = 270; // 26.0°C

static lcd1602_t lcd =
{
    .i2c = &hi2c1,
    .addr = 0x27 << 1,
};

static hw479_t hw479 =
{
	.htim = &htim2,
	.red_ch = TIM_CHANNEL_1,
	.green_ch = TIM_CHANNEL_2,
	.blue_ch = TIM_CHANNEL_3,
};

static ds3231_t ds3231 =
{
	.i2c = &hi2c1,
	.addr = 0xd0,
};

//static ds18b20_t ds18b20 =
//{
//	.htim = &htim6,
//	.pin = DS18B20_Pin,
//	.port = DS18B20_GPIO_Port,
//};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void log_uart_rx_byte(uint8_t byte);
static void process_uart_rx(void);
static void process_uart_rx_byte(uint8_t byte);
static void process_uart_command(const char *line);
static bool parse_temp_command(const char *line, const char *command, int16_t *temp);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static inline uint32_t TempToPWM(float x)
{
    const float slope = 10.0f;   // sensitivity (tune this)
    const uint32_t N = 0;
    const uint32_t M = 900;

    int32_t v = (int32_t)(x * slope) + N;

    if (v < (int32_t)N) v = N;
    if (v > (int32_t)M) v = M;

    return (uint32_t)v;
}

static void log_uart_rx_byte(uint8_t byte)
{
	uint16_t next_head = (uint16_t)(rx_log_head + 1U);

	if (next_head >= UART_RX_LOG_SIZE)
	{
		next_head = 0U;
	}

	if (next_head == rx_log_tail)
	{
		rx_overflow_count++;
		return;
	}

	rx_log[rx_log_head] = byte;
	rx_log_head = next_head;
}

static void process_uart_rx(void)
{
	static uint32_t printed_error_count = 0;
	static uint32_t printed_overflow_count = 0;

	while (rx_log_tail != rx_log_head)
	{
		uint8_t byte = rx_log[rx_log_tail];

		rx_log_tail = (uint16_t)(rx_log_tail + 1U);
		if (rx_log_tail >= UART_RX_LOG_SIZE)
		{
			rx_log_tail = 0U;
		}

		process_uart_rx_byte(byte);
	}

	if (printed_error_count != rx_error_count)
	{
		printed_error_count = rx_error_count;
		printf("[USART1] RX errors: %lu\r\n", rx_error_count);
	}

	if (printed_overflow_count != rx_overflow_count)
	{
		printed_overflow_count = rx_overflow_count;
		printf("[USART1] RX log overflows: %lu\r\n", rx_overflow_count);
	}
}

static void process_uart_rx_byte(uint8_t byte)
{
	if (byte == '\r' || byte == '\n')
	{
		if (uart_cmd_pos > 0U)
		{
			uart_cmd_line[uart_cmd_pos] = '\0';
			process_uart_command(uart_cmd_line);
			uart_cmd_pos = 0U;
		}

		return;
	}

	if (uart_cmd_pos < (UART_CMD_LINE_SIZE - 1U))
	{
		uart_cmd_line[uart_cmd_pos++] = (char)byte;
	}
	else
	{
		uart_cmd_pos = 0U;
		printf("[USART1] Command too long, dropping buffer\r\n");
	}
}

static void process_uart_command(const char *line)
{
	int16_t temp;

	printf("[USART1] Command: %s\r\n", line);

	if (parse_temp_command(line, "SET_MAX_TEMP", &temp))
	{
		max_temp = temp;
		printf("[USART1] SET_MAX_TEMP = %d (%d.%d C)\r\n",
				temp,
				temp / 10,
				abs(temp % 10));
	}
	else if (parse_temp_command(line, "SET_MIN_TEMP", &temp))
	{
		min_temp = temp;
		printf("[USART1] SET_MIN_TEMP = %d (%d.%d C)\r\n",
				temp,
				temp / 10,
				abs(temp % 10));
	}
	else
	{
		printf("[USART1] Unknown command\r\n");
	}
}

static bool parse_temp_command(const char *line, const char *command, int16_t *temp)
{
	const char *p = line;
	size_t command_len = strlen(command);
	uint32_t value = 0U;
	bool has_digit = false;

	while (*p == ' ' || *p == '\t')
	{
		p++;
	}

	if (strncmp(p, command, command_len) != 0)
	{
		return false;
	}

	p += command_len;

	if (*p != ' ' && *p != '\t')
	{
		return false;
	}

	while (*p == ' ' || *p == '\t')
	{
		p++;
	}

	while (*p >= '0' && *p <= '9')
	{
		has_digit = true;
		value = (value * 10U) + (uint32_t)(*p - '0');

		if (value > INT16_MAX)
		{
			return false;
		}

		p++;
	}

	while (*p == ' ' || *p == '\t')
	{
		p++;
	}

	if (!has_digit || *p != '\0')
	{
		return false;
	}

	*temp = (int16_t)value;
	return true;
}

void update(lcd1602_t *lcd, hw479_t *hw479, ds3231_t *ds3231) {
	ds3231_time_t t = ds3231_get_time(ds3231);
	lcd_1602_clear(lcd);

	lcd_1602_cur(lcd, 0, 0);
	lcd_1602_printf(lcd, "%02d:%02d %02d/%02d/20%02d", t.hour, t.minutes, t.dayofmonth, t.month, t.year);

	ds3231_force_temp_conv(ds3231);

	lcd_1602_cur (lcd, 1, 0);
	float temp = ds3231_get_temp(ds3231);
	int whole = (int)temp;
	int frac = (int)((temp - whole) * 10);
	lcd_1602_printf(lcd, "%d.%d C", whole, frac);

	int16_t tempInt = tempToFixed(temp);

	uint32_t v = TempToPWM(fabs(temp));

//	printf("Temperature = %d.%d C\r\n", whole, frac);
//	printf("Time = %02d:%02d %02d/%02d/20%02d\r\n", t.hour, t.minutes, t.dayofmonth, t.month, t.year);

	if (tempInt >= max_temp) {
		hw479_set_colors(hw479, 999, 0, 0);
	} else if (tempInt <= min_temp) {
		hw479_set_colors(hw479, 0, 0, 999);
	} else  {
		hw479_set_colors(hw479, 0, 0, 0);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  if (HAL_UART_Receive_IT(&huart1, &rx_byte, 1) != HAL_OK)
  {
      printf("USART1 RX IT start failed\r\n");
      Error_Handler();
  }
  else
  {
      printf("USART1 RX IT started\r\n");
  }

  lcd_1602_init(&lcd);
  lcd_1602_backlight_on(&lcd);
  lcd_1602_print(&lcd, "Initializing");

  hw479_init(&hw479);
//  ds18b20_init(&ds18b20);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_Delay(1000);

  printf("-------------------------\r\n");

  while (1)
  {
	process_uart_rx();
    update(&lcd, &hw479, &ds3231);
	process_uart_rx();
	HAL_Delay(750);
	process_uart_rx();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_8;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
    {
		rx_byte_count++;
		log_uart_rx_byte(rx_byte);

        if (HAL_UART_Receive_IT(&huart1, &rx_byte, 1) != HAL_OK)
        {
        	rx_error_count++;
        }
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
