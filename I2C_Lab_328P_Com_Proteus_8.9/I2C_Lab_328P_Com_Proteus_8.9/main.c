/*
 * I2C_Lab_328P_Com_Proteus_8.9.c
 *
 * Created: 31/12/2025 22:02:03
 * Author : franc
 */ 


/*
 * I2C_Lab_328P_Com_Proteus_8.9.c
 *
 * Created: 31/12/2025 22:02:03
 * Author : franc
 */

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

#include "twi.h"
#include "lcd_pcf8574.h"
#include "ds1307.h"

// ========= MUDE AQUI PARA “MUDAR VALORES NO SOFT” =========
#define SET_CLOCK_ON_BOOT  1   // 1 = grava no DS1307 ao ligar; 0 = só lê

// ========= LCD ENABLE/DISABLE (TESTE) =========
#define LCD_ENABLE  0          // 1 = usa LCD / 0 = desliga LCD (teste)

// Macros para não quebrar compilação quando LCD_ENABLE=0
#if LCD_ENABLE
  #define LCD_INIT()        lcd_init()
  #define LCD_CLEAR()       lcd_clear()
  #define LCD_GOTO(r,c)     lcd_goto((r),(c))
  #define LCD_PRINT(s)      lcd_print((s))
  #define LCD_HEX8(v)       lcd_print_hex8((v))
  #define LCD_U8_2D(v)      lcd_print_u8_2d((v))
#else
  #define LCD_INIT()        do{}while(0)
  #define LCD_CLEAR()       do{}while(0)
  #define LCD_GOTO(r,c)     do{}while(0)
  #define LCD_PRINT(s)      do{}while(0)
  #define LCD_HEX8(v)       do{}while(0)
  #define LCD_U8_2D(v)      do{}while(0)
#endif

// ========= DATA/HORA PARA AJUSTE (quando SET_CLOCK_ON_BOOT=1) =========
#define SET_HOUR   9
#define SET_MIN    3
#define SET_SEC    0

#define SET_DOW    4   // 1..7
#define SET_DAY    1
#define SET_MONTH  1
#define SET_YEAR   26  // 2026 -> 26
// =========================================================

static void lcd_print_time(const ds1307_dt_t *dt) {
    LCD_U8_2D(dt->hour); LCD_PRINT(":");
    LCD_U8_2D(dt->min);  LCD_PRINT(":");
    LCD_U8_2D(dt->sec);
}

static void lcd_print_date(const ds1307_dt_t *dt) {
    LCD_U8_2D(dt->day);   LCD_PRINT("/");
    LCD_U8_2D(dt->month); LCD_PRINT("/");
    LCD_U8_2D(dt->year);
}

int main(void) {
    // I2C init
    // Com 1MHz: use 50kHz
    twi_init(50000);

    // LCD init (usa macro: se LCD_ENABLE=0 vira "nada")
    LCD_INIT();
    LCD_GOTO(0,0); LCD_PRINT("I2C: PCF8574 +");
    LCD_GOTO(1,0); LCD_PRINT("DS1307 (ATmega)");
    _delay_ms(1200);
    LCD_CLEAR();

#if SET_CLOCK_ON_BOOT
    ds1307_dt_t set = {
        .sec = SET_SEC, .min = SET_MIN, .hour = SET_HOUR,
        .dow = SET_DOW, .day = SET_DAY, .month = SET_MONTH, .year = SET_YEAR
    };
    ds1307_set_datetime(&set);

    LCD_GOTO(0,0); LCD_PRINT("RTC ajustado!");
    _delay_ms(800);
    LCD_CLEAR();
#endif

    ds1307_dt_t now;

    while (1) {
        ds1307_read_datetime(&now);

        LCD_GOTO(0,0); LCD_PRINT("Hora: ");
        lcd_print_time(&now);
        LCD_PRINT("   ");

        LCD_GOTO(1,0); LCD_PRINT("Data: ");
        lcd_print_date(&now);
        LCD_PRINT("   ");

        LCD_GOTO(2,0); LCD_PRINT("PCF=0x");
        LCD_HEX8(PCF8574_ADDR);
        LCD_PRINT("  DS=0x68 ");

        LCD_GOTO(3,0); LCD_PRINT("SCL~50kHz @1MHz ");
        _delay_ms(300);
    }
}
