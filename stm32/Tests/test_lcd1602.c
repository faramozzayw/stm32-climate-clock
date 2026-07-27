#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "unity.h"
#include "drivers/lcd1602.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define LCD_ENABLE 0x04U
#define LCD_RS 0x01U
#define TRANSMISSIONS_PER_BYTE 6U
#define CURSOR_TRANSMISSION_COUNT TRANSMISSIONS_PER_BYTE
#define ROW_WRITE_TRANSMISSION_COUNT \
	(CURSOR_TRANSMISSION_COUNT +     \
		(LCD1602_COLUMN_COUNT * TRANSMISSIONS_PER_BYTE))

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

static void test_row_write_pads_to_display_width(void)
{
	lcd1602_t lcd = {0};
	size_t index;

	reset_transmissions();
	lcd_1602_write_row(&lcd, 0U, "Hi");

	TEST_ASSERT_TRUE(transmission_count == ROW_WRITE_TRANSMISSION_COUNT);
	TEST_ASSERT_TRUE((transmitted_states[CURSOR_TRANSMISSION_COUNT] & LCD_RS) != 0U);
	TEST_ASSERT_TRUE((transmitted_states[CURSOR_TRANSMISSION_COUNT] & LCD_ENABLE) == 0U);
	TEST_ASSERT_TRUE(transmitted_character(0U) == 'H');
	TEST_ASSERT_TRUE(transmitted_character(1U) == 'i');
	for (index = 2U; index < LCD1602_COLUMN_COUNT; index++)
	{
		TEST_ASSERT_TRUE(transmitted_character(index) == ' ');
	}
}

static void test_row_write_skips_unchanged_content(void)
{
	lcd1602_t lcd = {0};

	lcd_1602_write_row(&lcd, 0U, "unchanged");
	reset_transmissions();
	lcd_1602_write_row(&lcd, 0U, "unchanged");

	TEST_ASSERT_TRUE(transmission_count == 0U);
}

static void test_row_write_tracks_rows_independently(void)
{
	lcd1602_t lcd = {0};

	lcd_1602_write_row(&lcd, 0U, "same");
	reset_transmissions();
	lcd_1602_write_row(&lcd, 1U, "same");

	TEST_ASSERT_TRUE(transmission_count == ROW_WRITE_TRANSMISSION_COUNT);
}

static void test_row_write_updates_changed_content(void)
{
	lcd1602_t lcd = {0};

	lcd_1602_write_row(&lcd, 0U, "old");
	reset_transmissions();
	lcd_1602_write_row(&lcd, 0U, "new");

	TEST_ASSERT_EQUAL_UINT(ROW_WRITE_TRANSMISSION_COUNT, transmission_count);
	TEST_ASSERT_EQUAL_CHAR('n', transmitted_character(0U));
	TEST_ASSERT_EQUAL_CHAR('e', transmitted_character(1U));
	TEST_ASSERT_EQUAL_CHAR('w', transmitted_character(2U));
}

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_row_write_pads_to_display_width);
	RUN_TEST(test_row_write_skips_unchanged_content);
	RUN_TEST(test_row_write_tracks_rows_independently);
	RUN_TEST(test_row_write_updates_changed_content);
	return UNITY_END();
}
