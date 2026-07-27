#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "drivers/lcd1602.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define LCD_ENABLE 0x04U
#define LCD_RS 0x01U
#define TRANSMISSIONS_PER_BYTE 6U
#define CURSOR_TRANSMISSION_COUNT TRANSMISSIONS_PER_BYTE
#define ROW_WRITE_TRANSMISSION_COUNT \
	(CURSOR_TRANSMISSION_COUNT +     \
		(LCD1602_COLUMN_COUNT * TRANSMISSIONS_PER_BYTE))

#define CHECK(condition)                                                       \
	do                                                                         \
	{                                                                          \
		if (!(condition))                                                      \
		{                                                                      \
			printf("    CHECK failed at line %d: %s\n", __LINE__, #condition); \
			return false;                                                      \
		}                                                                      \
	} while (0)

static uint8_t transmitted_states[256];
static size_t transmission_count;

HAL_StatusTypeDef HAL_I2C_Master_Transmit(
	I2C_HandleTypeDef *i2c,
	uint16_t address,
	uint8_t *data,
	uint16_t size,
	uint32_t timeout)
{
	(void)i2c;
	(void)address;
	(void)timeout;
	if ((size != 1U) ||
		(transmission_count >= ARRAY_SIZE(transmitted_states)))
	{
		return HAL_ERROR;
	}
	transmitted_states[transmission_count++] = *data;
	return HAL_OK;
}

void HAL_Delay(uint32_t delay_ms)
{
	(void)delay_ms;
}

static void reset_transmissions(void)
{
	transmission_count = 0U;
	memset(transmitted_states, 0, sizeof(transmitted_states));
}

static char transmitted_character(size_t character_index)
{
	size_t offset =
		CURSOR_TRANSMISSION_COUNT +
		(character_index * TRANSMISSIONS_PER_BYTE);
	uint8_t high_nibble = transmitted_states[offset];
	uint8_t low_nibble =
		transmitted_states[offset + (TRANSMISSIONS_PER_BYTE / 2U)];

	return (char)((high_nibble & 0xF0U) | (low_nibble >> 4U));
}

static bool test_row_write_pads_to_display_width(void)
{
	lcd1602_t lcd = {0};
	size_t index;

	reset_transmissions();
	lcd_1602_write_row(&lcd, 0U, "Hi");

	CHECK(transmission_count == ROW_WRITE_TRANSMISSION_COUNT);
	CHECK((transmitted_states[CURSOR_TRANSMISSION_COUNT] & LCD_RS) != 0U);
	CHECK((transmitted_states[CURSOR_TRANSMISSION_COUNT] & LCD_ENABLE) == 0U);
	CHECK(transmitted_character(0U) == 'H');
	CHECK(transmitted_character(1U) == 'i');
	for (index = 2U; index < LCD1602_COLUMN_COUNT; index++)
	{
		CHECK(transmitted_character(index) == ' ');
	}
	return true;
}

static bool test_row_write_skips_unchanged_content(void)
{
	lcd1602_t lcd = {0};

	lcd_1602_write_row(&lcd, 0U, "unchanged");
	reset_transmissions();
	lcd_1602_write_row(&lcd, 0U, "unchanged");

	CHECK(transmission_count == 0U);
	return true;
}

static bool test_row_write_tracks_rows_independently(void)
{
	lcd1602_t lcd = {0};

	lcd_1602_write_row(&lcd, 0U, "same");
	reset_transmissions();
	lcd_1602_write_row(&lcd, 1U, "same");

	CHECK(transmission_count == ROW_WRITE_TRANSMISSION_COUNT);
	return true;
}

typedef bool (*test_function_t)(void);

typedef struct
{
	const char *name;
	test_function_t function;
} test_case_t;

int main(void)
{
	const test_case_t tests[] = {
		{"row writes pad to display width", test_row_write_pads_to_display_width},
		{"row writes skip unchanged content", test_row_write_skips_unchanged_content},
		{"row writes track rows independently", test_row_write_tracks_rows_independently},
	};
	size_t pos;
	size_t passed = 0U;

	for (pos = 0U; pos < ARRAY_SIZE(tests); pos++)
	{
		printf("[ RUN      ] %s\n", tests[pos].name);
		if (tests[pos].function())
		{
			printf("[       OK ] %s\n", tests[pos].name);
			passed++;
		}
		else
		{
			printf("[  FAILED  ] %s\n", tests[pos].name);
		}
	}

	printf("\n%zu/%zu tests passed\n", passed, ARRAY_SIZE(tests));
	return passed == ARRAY_SIZE(tests) ? 0 : 1;
}
