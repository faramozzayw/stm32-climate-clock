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

typedef struct
{
	I2C_HandleTypeDef *i2c;
	uint8_t addr;
	uint8_t state;
} lcd1602_t;

// public API
void lcd_1602_init(lcd1602_t *lcd);
void lcd_1602_cur(lcd1602_t *lcd, int row, int col);
void lcd_1602_print(lcd1602_t *lcd, const char *str);
void lcd_1602_printf(lcd1602_t *lcd, const char *fmt, ...);
void lcd_1602_clear(lcd1602_t *lcd);
void lcd_1602_cprint(lcd1602_t *lcd, const char *str);
void lcd_1602_backlight_on(lcd1602_t *lcd);
void lcd_1602_backlight_off(lcd1602_t *lcd);

#endif /* INC_LCD_H_ */
