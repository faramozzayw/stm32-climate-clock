#include <drivers/lcd1602.h>
#include "main.h"

// PCF8574 → LCD pin mapping (output latch bits)
// RS=register select, RW=read/write, EN=enable strobe, BL=backlight
#define LCD_RS 0b00000001
#define LCD_RW 0b00000010
#define LCD_ENABLE 0b00000100
#define LCD_BACKLIGHT 0b00001000

// Clear Display
// 0 0 0 0 0 0 0 1
#define LCD_CMD_CLEAR_DISPLAY 0b00000001

// Entry Mode Set
// 0 0 0 0 0 1 I/D S
// I/D = 1 -> increment cursor after each character
// S   = 0 -> do not shift display
#define LCD_CMD_ENTRY_MODE 0b00000110

// Display Control
// 0 0 0 0 1 D C B
// D = 1 -> display ON
// C = 0 -> cursor OFF
// B = 0 -> blinking OFF
#define LCD_CMD_DISPLAY_ON 0b00001100

// Function Set
// 0 0 1 DL N F x x
// DL = 0 -> 4-bit interface
// N  = 1 -> 2 display lines
// F  = 0 -> 5x8 font
#define LCD_CMD_FUNCTION_SET 0b00101000

// Initialization step 1
// Force controller into known state during startup.
// Part of the HD44780 4-bit initialization sequence.
#define LCD_CMD_INIT_RESET_1 0b00110011

// Initialization step 2
// Switch controller from 8-bit mode to 4-bit mode.
// Part of the HD44780 4-bit initialization sequence.
#define LCD_CMD_INIT_RESET_2 0b00110010

// -------------------- Low-level write --------------------
static void lcd_write_state(lcd1602_t *lcd, uint8_t data)
{
	lcd->state = data;
	HAL_I2C_Master_Transmit(lcd->i2c, lcd->addr, &lcd->state, 1, HAL_MAX_DELAY);
}

// -------------------- Pulse enable --------------------
static void lcd_pulse(lcd1602_t *lcd)
{
	lcd_write_state(lcd, lcd->state | LCD_ENABLE);
	HAL_Delay(1);

	lcd_write_state(lcd, lcd->state & ~LCD_ENABLE);
	HAL_Delay(1);
}

// -------------------- Send nibble --------------------
static void lcd_send_nibble(lcd1602_t *lcd, uint8_t nibble, uint8_t control)
{
	lcd->state =
		(lcd->state & LCD_BACKLIGHT) |
		((nibble & 0x0F) << 4) |
		control;

	lcd_write_state(lcd, lcd->state);
	lcd_pulse(lcd);
}

// -------------------- Command / Data --------------------
static void lcd_command(lcd1602_t *lcd, uint8_t cmd)
{
	lcd_send_nibble(lcd, cmd >> 4, 0);
	lcd_send_nibble(lcd, cmd & 0x0F, 0);
}

static void lcd_data(lcd1602_t *lcd, uint8_t data)
{
	lcd_send_nibble(lcd, data >> 4, LCD_RS);
	lcd_send_nibble(lcd, data & 0x0F, LCD_RS);
}

// -------------------- Public API --------------------

void lcd_1602_cur(lcd1602_t *lcd, int row, int col)
{
	switch (row)
	{
	case 0:
		col |= 0x80;
		break;
	case 1:
		col |= 0xC0;
		break;
	}

	lcd_command(lcd, col);
}

void lcd_1602_init(lcd1602_t *lcd)
{
	lcd->state = LCD_BACKLIGHT;
	lcd_write_state(lcd, lcd->state);

	lcd_command(lcd, LCD_CMD_INIT_RESET_1);
	lcd_command(lcd, LCD_CMD_INIT_RESET_2);

	lcd_command(lcd, LCD_CMD_FUNCTION_SET);
	lcd_command(lcd, LCD_CMD_DISPLAY_ON);
	lcd_command(lcd, LCD_CMD_ENTRY_MODE);
	lcd_command(lcd, LCD_CMD_CLEAR_DISPLAY);
}

void lcd_1602_print(lcd1602_t *lcd, const char *str)
{
	while (*str)
	{
		lcd_data(lcd, (uint8_t)*str++);
	}
}

void lcd_1602_printf(lcd1602_t *lcd, const char *fmt, ...)
{
	char buffer[64]; // adjust size based on your LCD needs

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	lcd_1602_print(lcd, buffer);
}

void lcd_1602_clear(lcd1602_t *lcd)
{
	lcd_command(lcd, LCD_CMD_CLEAR_DISPLAY);
}

void lcd_1602_cprint(lcd1602_t *lcd, const char *str)
{
	lcd_1602_clear(lcd);
	lcd_1602_print(lcd, str);
}

// -------------------- Backlight control --------------------
void lcd_1602_backlight_on(lcd1602_t *lcd)
{
	lcd->state |= LCD_BACKLIGHT;
	lcd_write_state(lcd, lcd->state);
}

void lcd_1602_backlight_off(lcd1602_t *lcd)
{
	lcd->state &= ~LCD_BACKLIGHT;
	lcd_write_state(lcd, lcd->state);
}
