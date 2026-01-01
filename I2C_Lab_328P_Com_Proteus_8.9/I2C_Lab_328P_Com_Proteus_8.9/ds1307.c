/*
 * ds1307.c
 *
 * Created: 31/12/2025 22:04:11
 *  Author: franc
 */ 

#define F_CPU 1000000UL

#include "ds1307.h"
#include "twi.h"

static uint8_t bcd_to_dec(uint8_t bcd) { return (bcd>>4)*10 + (bcd & 0x0F); }
static uint8_t dec_to_bcd(uint8_t dec) { return ((dec/10)<<4) | (dec%10); }

uint8_t ds1307_read_datetime(ds1307_dt_t *dt) {
	// Aponta registrador 0x00
	uint8_t st = twi_start(DS1307_ADDR, 0);
	if (st != 0x18) { twi_stop(); return st; }

	st = twi_write(0x00);
	if (st != 0x28) { twi_stop(); return st; }

	// Repeated start para leitura
	st = twi_start(DS1307_ADDR, 1);
	if (st != 0x40) { twi_stop(); return st; }

	uint8_t b;

	twi_read_ack(&b);  dt->sec   = bcd_to_dec(b & 0x7F);
	twi_read_ack(&b);  dt->min   = bcd_to_dec(b);
	twi_read_ack(&b);  dt->hour  = bcd_to_dec(b & 0x3F);
	twi_read_ack(&b);  dt->dow   = bcd_to_dec(b);
	twi_read_ack(&b);  dt->day   = bcd_to_dec(b);
	twi_read_ack(&b);  dt->month = bcd_to_dec(b);
	twi_read_nack(&b); dt->year  = bcd_to_dec(b);

	twi_stop();
	return 0; // ok
}

uint8_t ds1307_set_datetime(const ds1307_dt_t *dt) {
	uint8_t st = twi_start(DS1307_ADDR, 0);
	if (st != 0x18) { twi_stop(); return st; }

	st = twi_write(0x00);
	if (st != 0x28) { twi_stop(); return st; }

	// CH (clock halt) = 0 => bit7 do seconds deve ser 0
	st = twi_write(dec_to_bcd(dt->sec) & 0x7F); if (st != 0x28) { twi_stop(); return st; }
	st = twi_write(dec_to_bcd(dt->min));        if (st != 0x28) { twi_stop(); return st; }
	st = twi_write(dec_to_bcd(dt->hour));       if (st != 0x28) { twi_stop(); return st; }
	st = twi_write(dec_to_bcd(dt->dow));        if (st != 0x28) { twi_stop(); return st; }
	st = twi_write(dec_to_bcd(dt->day));        if (st != 0x28) { twi_stop(); return st; }
	st = twi_write(dec_to_bcd(dt->month));      if (st != 0x28) { twi_stop(); return st; }
	st = twi_write(dec_to_bcd(dt->year));       if (st != 0x28) { twi_stop(); return st; }

	twi_stop();
	return 0; // ok
}
