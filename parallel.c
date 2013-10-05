/*
 * This file is part of the frser-atmega644 project.
 *
 * Copyright (C) 2010 Urja Rannikko <urjaman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "main.h"
#include "flash.h"
#include "parallel.h"
#include "uart.h"

static uint8_t parallel_databus_read(void) {
	uint8_t rv;
	rv = (PINC & 0x3F)<<2;
	rv |= (PIND & 0xC0)>>6;
	return rv;
}

static void parallel_databus_tristate(void) {
	DDRC &= ~(0x3F);
	DDRD &= ~(0xC0);
	PORTC &= ~(0x3F);
	PORTD &= ~(0xC0);
}

static void parallel_databus_output(uint8_t data) {
	PORTC = ((PORTC & 0xC0) | ((data>>2) & 0x3F));
	PORTD = ((PORTD & 0x3F) | ((data<<6) & 0xC0));
	DDRC |= 0x3F;
	DDRD |= 0xC0;
}

static void parallel_chip_enable(void) {
	DDRC |= _BV(6);
	PORTC &= ~_BV(6);
}


/*static void parallel_chip_disable(void) {
	PORTC |= _BV(6); 
}*/

static void parallel_output_enable(void) {
	DDRA |= _BV(7);
	PORTA &= ~_BV(7);
}

static void parallel_output_disable(void) {
	PORTA |= _BV(7);
}

static void parallel_addrbus_out(void) {
	// pa123456,
	DDRA |= 0x7E;
	// pb01234567
	DDRB = 0xFF;
	// pc7
	DDRC |= _BV(7);
	// pd2345
	DDRD |= (_BV(2)|_BV(3)|_BV(4)|_BV(5));
	}

static void parallel_addrbus_safe(void) {  // turn off drivers on A13 and A11 - safe to attach FWH/LPC chips
	DDRA &= ~(_BV(3)|_BV(6));
	}

#define parallel_pinmapper(to,from,bit_to,bit_from) \
	asm ( "bst %1, " #bit_from "\n\t" \
	      "bld %0, " #bit_to "\n\t" \
	      : "+r" (to) : "r" (from))



static void parallel_setaddr(uint32_t addr) {
	uint8_t ad_a,ad_b,ad_c,port; // split address to 8bit parts
	asm(
	"mov %0, %A3"	"\n\t"
	"mov %1, %B3"	"\n\t"
	"mov %2, %C3"	"\n\t"
	: "=r" (ad_a), "=r" (ad_b), "=r" (ad_c) : "r" (addr) );
	port = PORTA;
	parallel_pinmapper(port,ad_c,1,17-16);
	parallel_pinmapper(port,ad_b,2,14 -8);
	parallel_pinmapper(port,ad_b,3,13 -8);
	parallel_pinmapper(port,ad_b,4, 8 -8);
	parallel_pinmapper(port,ad_b,5, 9 -8);
	parallel_pinmapper(port,ad_b,6,11 -8);
	PORTA = port;
	//port = 0; (all bits are set via bld, no need to clear)
	parallel_pinmapper(port,ad_c,0,18-16);
	parallel_pinmapper(port,ad_c,1,16-16);
	parallel_pinmapper(port,ad_b,2,15 -8);
	parallel_pinmapper(port,ad_b,3,12 -8);
	parallel_pinmapper(port,ad_a,4, 7 -0);
	parallel_pinmapper(port,ad_a,5, 6 -0);
	parallel_pinmapper(port,ad_a,6, 5 -0);
	parallel_pinmapper(port,ad_a,7, 4 -0);
	PORTB = port;
	port = PORTC;
	parallel_pinmapper(port,ad_b,7,10 -8);
	PORTC = port;
	port = PORTD;
	parallel_pinmapper(port,ad_a,2, 3 -0);
	parallel_pinmapper(port,ad_a,3, 2 -0);
	parallel_pinmapper(port,ad_a,4, 1 -0);
	parallel_pinmapper(port,ad_a,5, 0 -0);
	PORTD = port;
}

static void parallel_pulse_we(void) {
	asm volatile(
	"nop\n\t"
	"sbi %0, 0\n\t"
	"nop\n\t"
	"sbi %0, 0\n\t"
	::
	"I" (_SFR_IO_ADDR(PINA))
	);
}

static void parallel_read_init(void) {
	parallel_databus_tristate();
	parallel_output_enable();
	parallel_chip_enable();
	parallel_addrbus_out();
}


// assume chip enabled & output enabled & databus tristate
static uint8_t parallel_readcycle(uint32_t addr) {
	parallel_setaddr(addr);
	return parallel_databus_read();
}


uint8_t parallel_test(void) {
	DDRA &= ~_BV(3);
	PORTA |= _BV(3);
	_delay_us(1);
	if (!(PINA&_BV(3))) { // detected LPC/FWH chip - A13 wont rise because its GND
		PORTA &= ~_BV(3); // A13 to zero (GND)
		PORTA |= _BV(6);  // A11 to one (Vcc)
		DDRA |= _BV(3)|_BV(6); // drive them
		return 0; // dont try parallel access
		}
	DDRA |= _BV(3); // allow driving
	return 1; // all seems ok, go for it  :)
	}

void parallel_readn(uint32_t addr, uint32_t len) {
	if (parallel_test()) {
		parallel_read_init();
		do {
		SEND(parallel_readcycle(addr++));
		} while(--len);
		// safety features
		parallel_output_disable();
		parallel_addrbus_safe();
	} else {
		do {
		SEND(0xFF);
		} while(--len);
	}
}

// assume nothing, and perform single cycle
uint8_t parallel_read(uint32_t addr) {
	uint8_t data;
	if (parallel_test()) {
		parallel_read_init();
		parallel_setaddr(addr);
		data = parallel_databus_read();
		parallel_output_disable();
		parallel_addrbus_safe();
	} else {
		data = 0xFF;
	}
	return data;
}

// assume nothing, perform single cycle
void parallel_write(uint32_t addr, uint8_t data) {
	if (parallel_test()) {
		PORTA |= _BV(0);
		DDRA |= _BV(0); // drive !WE pin
		parallel_output_disable();
		parallel_addrbus_out();
		parallel_chip_enable();
		parallel_databus_output(data);
		parallel_setaddr(addr);
		parallel_pulse_we();
		parallel_addrbus_safe();
	}
}