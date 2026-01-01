/*
 * ds1307.h
 *
 * Created: 31/12/2025 22:03:42
 *  Author: franc
 */ 


#ifndef DS1307_H
#define DS1307_H

#include <stdint.h>

#define DS1307_ADDR 0x68

typedef struct {
	uint8_t sec;   // 0-59
	uint8_t min;   // 0-59
	uint8_t hour;  // 0-23
	uint8_t dow;   // 1-7 (dia da semana)
	uint8_t day;   // 1-31
	uint8_t month; // 1-12
	uint8_t year;  // 0-99 (ex: 25 = 2025)
} ds1307_dt_t;

uint8_t ds1307_read_datetime(ds1307_dt_t *dt);
uint8_t ds1307_set_datetime(const ds1307_dt_t *dt);

#endif
