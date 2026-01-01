/*
 * lcd_pcf8574.c
 *
 * Created: 31/12/2025 22:05:06
 *  Author: franc
 */ 



#define F_CPU 1000000UL
#include <util/delay.h>
#include "lcd_pcf8574.h"
#include "twi.h"

// Mapeamento (padrão backpack):
// P0 RS
// P1 RW
// P2 EN
// P3 BL
// P4 D4
// P5 D5
// P6 D6
// P7 D7

#define LCD_RS (1<<0)
#define LCD_RW (1<<1)
#define LCD_EN (1<<2)
#define LCD_BL (1<<3)

static uint8_t pcf_state = LCD_BL; // backlight ligado (opcional)

static void pcf_write(uint8_t v) {
	// escreve 1 byte no PCF8574
	uint8_t st = twi_start(PCF8574_ADDR, 0);
	if (st != 0x18) { twi_stop(); return; }
	twi_write(v);
	twi_stop();
}

static void lcd_pulse_enable(uint8_t v) {
	pcf_write(v | LCD_EN);
	_delay_us(1);
	pcf_write(v & ~LCD_EN);
	_delay_us(50);
}

static void lcd_write4(uint8_t nibble, uint8_t rs) {
	// nibble em bits 7..4 do "pacote" do PCF
	uint8_t v = pcf_state;
	if (rs) v |= LCD_RS;
	v &= ~LCD_RW; // sempre write

	// joga nibble nos bits P4..P7
	v &= 0x0F;
	v |= (nibble << 4);

	pcf_write(v);
	lcd_pulse_enable(v);
}

static void lcd_send(uint8_t byte, uint8_t rs) {
	lcd_write4((byte >> 4) & 0x0F, rs);
	lcd_write4(byte & 0x0F, rs);
}

static void lcd_cmd(uint8_t c) {
	lcd_send(c, 0);
	if (c == 0x01 || c == 0x02) _delay_ms(2);
}

static void lcd_data(uint8_t d) {
	lcd_send(d, 1);
}

void lcd_init(void) {
	_delay_ms(50);

	// sequência init 4-bit do HD44780
	lcd_write4(0x03, 0); _delay_ms(5);
	lcd_write4(0x03, 0); _delay_us(150);
	lcd_write4(0x03, 0); _delay_us(150);
	lcd_write4(0x02, 0); _delay_us(150); // 4-bit

	// Function set: 4-bit, 2 linhas (20x4 usa “2-line mode”), font 5x8
	lcd_cmd(0x28);
	// Display ON, cursor OFF, blink OFF
	lcd_cmd(0x0C);
	// Entry mode
	lcd_cmd(0x06);
	// Clear
	lcd_clear();
}

void lcd_clear(void) {
	lcd_cmd(0x01);
}

void lcd_goto(uint8_t row, uint8_t col) {
	// endereços típicos 20x4:
	// row0: 0x00
	// row1: 0x40
	// row2: 0x14
	// row3: 0x54
	static const uint8_t base[] = {0x00, 0x40, 0x14, 0x54};
	if (row > 3) row = 3;
	lcd_cmd(0x80 | (base[row] + col));
}

void lcd_print(const char *s) {
	while (*s) lcd_data((uint8_t)*s++);
}

void lcd_print_u8_2d(uint8_t v) {
	// imprime sempre 2 dígitos (00..99)
	lcd_data('0' + (v/10));
	lcd_data('0' + (v%10));
}

//********************************************************************************************************************
void lcd_putc(char c) {
	lcd_data((uint8_t)c); // lcd_data é interna, mas aqui dentro pode usar
}

void lcd_print_hex8(uint8_t v) {
	const char hex[] = "0123456789ABCDEF";
	lcd_putc(hex[(v >> 4) & 0x0F]);
	lcd_putc(hex[v & 0x0F]);
}


//********************************************************************************************************************