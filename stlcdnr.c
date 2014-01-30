/*
$Id:$

ST7565 LCD library! - No RAM buffer edition (NR)

Copyright (C) 2010 Limor Fried, Adafruit Industries
Copyright (C) 2014 Urja Rannikko <urjaman@gmail.com>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 // some of this code was written by <cstone@pobox.com> originally; it is in the public domain.
*/
#include "main.h" // Most important: define F_CPU
#include <avr/pgmspace.h> 
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
// These only for debug
#include "console.h"
#include "uart.h"

#include "stlcdhw.h"
#include "stlcdnr.h"


/* HW config. */
#define SID_DDR DDRA
#define SID_PIN PINA
#define SID_PORT PORTA
#define SID 4

#define SCLK_DDR DDRA
#define SCLK_PIN PINA
#define SCLK_PORT PORTA
#define SCLK 3

#define A0_DDR DDRA
#define A0_PIN PINA
#define A0_PORT PORTA
#define A0 2

#define RST_DDR DDRA
#define RST_PIN PINA
#define RST_PORT PORTA
#define RST 1

#define CS_DDR DDRA
#define CS_PIN PINA
#define CS_PORT PORTA
#define CS 0

/* Internal state */
static uint8_t lcd_char_x = 0;
static uint8_t lcd_char_y = 0;

/* Internal functions */
static void st7565_clear(void);
static void st7565_init(void);
static void spiwrite(uint8_t c);
static void st7565_command(uint8_t c);
static void st7565_data(uint8_t c);
static void st7565_gotoxy(uint8_t x, uint8_t y);

/* Init Debug is optional. */
#ifndef SENDSTR
#define SENDSTR
#endif

#define nop asm volatile ("nop\n\t")

void lcd_init(void)
{
    SENDSTR("SETUP\r\n");
    st7565_init();
    SENDSTR("INIT\r\n");
    st7565_command(CMD_DISPLAY_ON);
    SENDSTR("DISPLAY_ON\r\n");
    st7565_command(CMD_SET_ALLPTS_NORMAL);
    SENDSTR("ALLPTS_NORMAL\r\n");
    st7565_set_contrast(0x16);
    SENDSTR("CONTRAST 0X16\r\n");
    lcd_clear();
    SENDSTR("LCD_INIT DONE\r\n");
}

void lcd_gotoxy(uint8_t x, uint8_t y)
{
    lcd_char_x = x;
    lcd_char_y = y;
}

void lcd_clear(void)
{
    st7565_clear();
    lcd_char_x = 0;
    lcd_char_y = 0;
}


static void st7565_init(void) {
  // set pin directions
  SID_DDR |= _BV(SID);
  SCLK_DDR |= _BV(SCLK);
  A0_DDR |= _BV(A0);
  RST_DDR |= _BV(RST);
  CS_DDR |= _BV(CS);
  
  // toggle RST low to reset; CS low so it'll listen to us
  CS_PORT &= ~_BV(CS);
  RST_PORT &= ~_BV(RST);
  _delay_ms(500);
  RST_PORT |= _BV(RST);

  // LCD bias select
  st7565_command(CMD_SET_BIAS_7);
  // ADC select
  st7565_command(CMD_SET_ADC_NORMAL);
  // SHL select
  st7565_command(CMD_SET_COM_NORMAL);
  // Initial display line
  st7565_command(CMD_SET_DISP_START_LINE);

  // turn on voltage converter (VC=1, VR=0, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x4);
  // wait for 50% rising
  _delay_ms(50);

  // turn on voltage regulator (VC=1, VR=1, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x6);
  // wait >=50ms
  _delay_ms(50);

  // turn on voltage follower (VC=1, VR=1, VF=1)
  st7565_command(CMD_SET_POWER_CONTROL | 0x7);
  // wait
  _delay_ms(10);

  // set lcd operating voltage (regulator resistor, ref voltage resistor)
  st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);

}

static void spiwrite(uint8_t c) {
  int8_t i;
  for (i=7; i>=0; i--) {
    SCLK_PORT &= ~_BV(SCLK);
    if (c & 0x80)
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    c = c<<1;
    nop;
    SCLK_PORT |= _BV(SCLK);
    nop;
    nop;
  }
}

static void st7565_command(uint8_t c) {
  A0_PORT &= ~_BV(A0);
  spiwrite(c);
}

static void st7565_data(uint8_t c) {
  A0_PORT |= _BV(A0);
  spiwrite(c);
}

static void st7565_clear(void) {
  uint8_t p, c;
  for(p = 0; p < 8; p++) {
    st7565_command(CMD_SET_PAGE | p);
    spiwrite(CMD_SET_COLUMN_LOWER | (0x0 & 0xf)); /* Already in command mode. */
    spiwrite(CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));
    spiwrite(CMD_RMW);
    st7565_data(0xff);
    for(c = 0; c < 128; c++) {
	spiwrite(0); // Already in data mode as above
    }
  }
}

void st7565_set_contrast(uint8_t val)
{
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

static void st7565_gotoxy(uint8_t x, uint8_t y) /* This is the hardware gotoxy */
{
	const uint8_t pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };
	uint8_t cs = 1+(LCD_CHARW*x);
	st7565_command(CMD_SET_PAGE | pagemap[y]);
	spiwrite(CMD_SET_COLUMN_LOWER | (cs & 0xf));
	spiwrite(CMD_SET_COLUMN_UPPER | ((cs >> 4) & 0xf));
	spiwrite(CMD_RMW);
}

void lcd_write_block_P(const PGM_P buffer, uint8_t w, uint8_t h)
{
	uint8_t ye = lcd_char_y+h;
	uint8_t we = (lcd_char_x+w)*LCD_CHARW;
	if (we > (LCD_CHARW*LCD_MAXX)) return; /* Dont waste time writing clipped stuff (this would break lines if we clipped). */
	if (ye > LCD_MAXY) ye = LCD_MAXY; /* This can be safely clipped... */
	for (uint8_t y=lcd_char_y;y<ye;y++) {
		st7565_gotoxy(lcd_char_x,y);
		for (uint8_t x=lcd_char_x*LCD_CHARW;x<we;x++) {
			uint8_t d = pgm_read_byte(buffer);
			buffer++;
			st7565_data(d);
		}
	}
	lcd_char_x += w;
	if (lcd_char_x > LCD_MAXX) lcd_char_x = LCD_MAXX; /* saturate */

}

void lcd_write_block(const uint8_t *buffer, uint8_t w, uint8_t h)
{
	uint8_t ye = lcd_char_y+h;
	uint8_t we = (lcd_char_x+w)*LCD_CHARW;
	if (we > (LCD_CHARW*LCD_MAXX)) return;;
	if (ye > LCD_MAXY) ye = LCD_MAXY; /* This can be safely clipped... */
	for (uint8_t y=lcd_char_y;y<ye;y++) {
		st7565_gotoxy(lcd_char_x,y);
		for (uint8_t x=lcd_char_x*LCD_CHARW;x<we;x++) {
			uint8_t d = *buffer;
			buffer++;
			st7565_data(d);
		}
	}
	lcd_char_x += w;
	if (lcd_char_x > LCD_MAXX) lcd_char_x = LCD_MAXX; /* saturate */
}

// Font8x8 is generated from the font at https://github.com/dhepper/font8x8 (Public Domain == i dont even need to mention this)
#include "font8x8.c"

void lcd_putchar(unsigned char c)
{
	PGM_P block;
	if (c>=0xA0) {
		block = (const char*)&(font8x8_ext[(c-0xA0)*LCD_CHARW]);
	} else if ((c>= 0x20)&&(c <= 0x7E)) {
		block = (const char*)&(font8x8_ascii[(c-0x20)*LCD_CHARW]);
	} else {
		block = (const char*)font8x8_ascii; // Space
	}
	lcd_write_block_P(block,1,1);
}

void lcd_puts(const unsigned char * str)
{
start:
        if (*str) lcd_putchar(*str);
        else return;
        str++;
        goto start;
}

void lcd_puts_P(PGM_P str)
{
        unsigned char c;
start:
        c = pgm_read_byte(str);
        if (c) lcd_putchar(c);
        else return;
        str++;
        goto start;
}

