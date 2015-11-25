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

struct command_t {
	PGM_P name;
	void(*function)(void);
};

extern const struct command_t appdb[] PROGMEM;

void *find_appdb(unsigned char* cmd);
void echo_cmd(void);
void help_cmd(void);
void calc_cmd(void);
void bljump_cmd(void);
void lcdr_cmd(void);
void lcdbr_cmd(void);
void lcdw_cmd(void);
void lcdc_cmd(void);
void lcdbg_cmd(void);
void lbench_cmd(void);
void lgfxt_cmd(void);
void blset_cmd(void);
void ldw_cmd(void);
void fader_cmd(void);
void lcdclr_cmd(void);
void lcdc2_cmd(void);
