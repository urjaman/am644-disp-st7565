/*
 * This file is part of the frser-atmega644 project.
 *
 * Copyright (C) 2010,2011,2013 Urja Rannikko <urjaman@gmail.com>
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
#include "flash.h"
#include "udelay.h"
#include "frser.h"
#include "ciface.h"
#include "typeu.h"


void frser_main(void) {
	jmp_buf uart_timeout;
	for(;;) {
		uint8_t op;
		uart_set_timeout(NULL);
		op = RECEIVE();
		if (op == 0x20) { 
			ciface_main();
			continue;
		}
		if (op > S_MAXCMD) {
			/* This is a pretty futile case as in that we shouldnt get
			these commands at all with the supported cmd bitmap system */
			/* Still better to say something vs. nothing.		   */
			SEND(S_NAK);
			continue;
		}
		if (setjmp(uart_timeout)) {
			/* We might be in the middle of an SPI operation or otherwise
			   in a weird state. Re-init and hope for best. */
			SEND(S_NAK); /* Tell of a problem. */
			continue;
		}
	}
}
