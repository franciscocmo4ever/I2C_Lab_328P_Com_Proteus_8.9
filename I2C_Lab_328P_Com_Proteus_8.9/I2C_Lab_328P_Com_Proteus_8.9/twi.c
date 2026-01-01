/*
 * twi.c
 *
 * Created: 31/12/2025 22:03:11
 *  Author: franc
 */ 



#define F_CPU 1000000UL   // ajuste conforme teu clock real
#include <avr/io.h>
#include "twi.h"

// Retorna status (TWSR & 0xF8)
uint8_t twi_status(void) {
	return (TWSR & 0xF8);
}

void twi_init(uint32_t scl_hz) {
	// Prescaler = 1
	TWSR &= ~((1<<TWPS0) | (1<<TWPS1));

	// Fórmula do datasheet:
	// SCL = F_CPU / (16 + 2*TWBR*prescaler)
	// => TWBR = ((F_CPU/SCL)-16)/2
	//
	// Obs: com F_CPU=1MHz, SCL máximo ~62.5kHz.
	// Então use 50kHz (bom e estável).
	uint32_t twbr_val = ((F_CPU / scl_hz) - 16) / 2;
	if (twbr_val > 255) twbr_val = 255;
	TWBR = (uint8_t)twbr_val;

	// Habilita TWI
	TWCR = (1<<TWEN);
}

uint8_t twi_start(uint8_t address_7bit, uint8_t read) {
	// Envia START
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));

	uint8_t st = twi_status();
	if (st != 0x08 && st != 0x10) {
		return st; // START ou REP START falhou
	}

	// SLA+W ou SLA+R
	uint8_t sla = (address_7bit<<1) | (read ? 1 : 0);
	TWDR = sla;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));

	st = twi_status();
	// 0x18 = SLA+W ACK
	// 0x40 = SLA+R ACK
	return st;
}

void twi_stop(void) {
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	// não precisa esperar terminar; mas para estudo dá pra esperar:
	while (TWCR & (1<<TWSTO));
}

uint8_t twi_write(uint8_t data) {
	TWDR = data;
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	return twi_status(); // 0x28 = DATA ACK
}

uint8_t twi_read_ack(uint8_t *data) {
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
	while (!(TWCR & (1<<TWINT)));
	*data = TWDR;
	return twi_status(); // 0x50 = DATA recebido, ACK retornado
}

uint8_t twi_read_nack(uint8_t *data) {
	TWCR = (1<<TWINT) | (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	*data = TWDR;
	return twi_status(); // 0x58 = DATA recebido, NACK retornado
}
