/*
 * lcd.h
 *
 *  Created on: Jun 16, 2026
 *      Author: faram
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32l0xx_hal.h"
#include <stdarg.h>
#include <stdio.h>

#define LCD1602_ROW_COUNT 2U
#define LCD1602_COLUMN_COUNT 16U
#define LCD1602_COLUMN_COUNT_WITH_NULL_TERMINATOR 17U

typedef struct
{
	I2C_HandleTypeDef *i2c;
	uint8_t addr;
	uint8_t state;
	char displayed_rows[LCD1602_ROW_COUNT][LCD1602_COLUMN_COUNT + 1U];
	uint8_t valid_rows;
} lcd1602_t;

#ifdef __cplusplus
extern "C"
{
#endif

	// public API
	void lcd_1602_init(lcd1602_t *lcd);
	void lcd_1602_cur(lcd1602_t *lcd, int row, int col);
	void lcd_1602_print(lcd1602_t *lcd, const char *str);
	void lcd_1602_printf(lcd1602_t *lcd, const char *fmt, ...);

	/**
 * @brief Overwrite one display row without clearing the display.
 *
 * Text is truncated or space-padded to the display width. The write is
 * skipped when the row already contains the same text.
 */
	void lcd_1602_write_row(lcd1602_t *lcd, uint8_t row, const char *text);

	void lcd_1602_clear(lcd1602_t *lcd);
	void lcd_1602_cprint(lcd1602_t *lcd, const char *str);
	void lcd_1602_backlight_on(lcd1602_t *lcd);
	void lcd_1602_backlight_off(lcd1602_t *lcd);

#ifdef __cplusplus
}
#endif

#endif /* INC_LCD_H_ */
