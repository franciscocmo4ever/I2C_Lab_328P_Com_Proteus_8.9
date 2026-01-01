/*
 * lcd_pcf8574.h
 *
 * Created: 31/12/2025 22:04:50
 *  Author: franc
 */ 



#ifndef LCD_PCF8574_H
#define LCD_PCF8574_H

#include <stdint.h>

#ifndef PCF8574_ADDR
#define PCF8574_ADDR 0x27  // MUDE AQUI se teu PCF for 0x20..0x27
#endif

void lcd_init(void);
void lcd_clear(void);
void lcd_goto(uint8_t row, uint8_t col);
void lcd_print(const char *s);
void lcd_print_u8_2d(uint8_t v); // imprime 00-99 (didático)

void lcd_putc(char c);
void lcd_print_hex8(uint8_t v);


#endif
