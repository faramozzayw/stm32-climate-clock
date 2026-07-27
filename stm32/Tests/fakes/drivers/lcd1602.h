#ifndef TEST_FAKE_LCD1602_H_
#define TEST_FAKE_LCD1602_H_

#include <stdint.h>

#define LCD1602_COLUMN_COUNT 16U

typedef struct
{
	uint8_t unused;
} lcd1602_t;

void lcd_1602_init(lcd1602_t *lcd);
void lcd_1602_print(lcd1602_t *lcd, const char *str);
void lcd_1602_write_row(lcd1602_t *lcd, uint8_t row, const char *text);
void lcd_1602_backlight_on(lcd1602_t *lcd);

#endif /* TEST_FAKE_LCD1602_H_ */
