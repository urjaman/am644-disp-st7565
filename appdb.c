/*
 * This file is part of the am644-disp project.
 *
 * Copyright (C) 2013,2014 Urja Rannikko <urjaman@gmail.com>
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
#include "ciface.h"
#include "console.h"
#include "appdb.h"

const unsigned char echostr[] PROGMEM = "ECHO";
const unsigned char bljumpstr[] PROGMEM = "BLJUMP";
const unsigned char lcdistr[] PROGMEM = "LINIT";
const unsigned char lcdcstr[] PROGMEM = "LCHAR";
const unsigned char lcdc2str[] PROGMEM = "LUCHR";
const unsigned char lcdclrstr[] PROGMEM = "LCLR";
const unsigned char lcdbrstr[] PROGMEM = "LBRI";
const unsigned char lcdwstr[] PROGMEM = "LW";
const unsigned char ldwstr[]  PROGMEM = "LDW";
const unsigned char lcdbgstr[] PROGMEM = "LBG";
const unsigned char lbenchstr[] PROGMEM = "LBENCH";
const unsigned char lgfxtstr[] PROGMEM = "LGFXT";
const unsigned char blsetstr[] PROGMEM = "BLSET";
const unsigned char calcstr[] PROGMEM = "CALC";
const unsigned char helpstr[] PROGMEM = "?";
const unsigned char faderstr[] PROGMEM = "FADE";

const struct command_t appdb[] PROGMEM = {
	{(PGM_P)echostr, &(echo_cmd)},
	{(PGM_P)bljumpstr, &(bljump_cmd)},
	{(PGM_P)lcdistr, &(lcdr_cmd)},
	{(PGM_P)lcdclrstr, &(lcdclr_cmd)},
	{(PGM_P)lcdcstr, &(lcdc_cmd)},
	{(PGM_P)lcdc2str, &(lcdc2_cmd)},
	{(PGM_P)lcdbrstr, &(lcdbr_cmd)},
	{(PGM_P)lcdwstr, &(lcdw_cmd)},
	{(PGM_P)ldwstr, &(ldw_cmd)},
	{(PGM_P)lcdbgstr, &(lcdbg_cmd)},
	{(PGM_P)lbenchstr, &(lbench_cmd)},
	{(PGM_P)lgfxtstr, &(lgfxt_cmd)},
	{(PGM_P)blsetstr, &(blset_cmd)},
	{(PGM_P)faderstr, &(fader_cmd)},
	{(PGM_P)calcstr, &(calc_cmd)},
	{(PGM_P)helpstr, &(help_cmd)},
	{NULL,NULL}
};

void invalid_command(void) {
	sendstr(tokenptrs[0]);
	sendstr_P(PSTR(": not found"));
	}

void *find_appdb(unsigned char* cmd) {
	unsigned char i;
	const struct command_t * ctptr;
	PGM_P name;
	void* fp;
	for(i=0;;i++) {
		ctptr = &(appdb[i]);
		name = (PGM_P)pgm_read_word(&(ctptr->name));
		fp = (void*)pgm_read_word(&(ctptr->function));
		if (!name) break;
		if (strcmp_P((char*)cmd,name) == 0) {
			return fp;
			}
	}
	return &(invalid_command);
}


