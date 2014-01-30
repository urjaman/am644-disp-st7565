/*
 * This file is part of the am644-disp project.
 *
 * Copyright (C) 2010,2014 Urja Rannikko <urjaman@gmail.com>
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
#include "uart.h"
#include "ciface.h"
static void mainloop(void);

int main(void) {
	cli();
	uart_init();
	power_adc_disable();
	power_timer0_disable();
	power_timer2_disable();
	power_twi_disable();
	mainloop();
}

/* This was frser-main... */
static void mainloop(void) {
	uart_set_timeout(NULL);
	for(;;) {
		uint8_t op;
		op = RECEIVE();
		if (op == 0x20) { 
			ciface_main();
			continue;
		}
		/* Here is a possibility to have an alternative binary interface so I'll just leave this blank... */
	
	}
}

