/*
$Id:$

ST7565 LCD library! - modified for the no big ram buffer edition

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
*/

#include "main.h"
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h> 
#include <string.h>
#include "glcd.h"

// the most basic function, set a single pixel
void setpixel(struct drawdata *d, uint8_t x, uint8_t y, uint8_t color) {
  if ((x >= (d->w*LCD_CHARW)) || (y >= (d->h*LCD_CHARH)))
    return;

  // x is which column
  if (color) 
    d->d[x+ (y/8)*(d->w*LCD_CHARW)] |= _BV(7-(y%8));  
  else
    d->d[x+ (y/8)*(d->w*LCD_CHARW)] &= ~_BV(7-(y%8)); 
}



// bresenham's algorithm - thx wikpedia
void drawline(struct drawdata *d,
	      uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, 
	      uint8_t color) {

  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if (x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  uint8_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int8_t err = dx / 2;
  int8_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<x1; x0++) {
    if (steep) {
      setpixel(d, y0, x0, color);
    } else {
      setpixel(d, x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

// filled rectangle
void fillrect(struct drawdata *d,
	      uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
	      uint8_t color) {

  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    for (uint8_t j=y; j<y+h; j++) {
      setpixel(d, i, j, color);
    }
  }
}


// draw a rectangle
void drawrect(struct drawdata *d,
	      uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
	      uint8_t color) {
  // stupidest version - just pixels - but fast with internal buffer!
  for (uint8_t i=x; i<x+w; i++) {
    setpixel(d, i, y, color);
    setpixel(d, i, y+h-1, color);
  }
  for (uint8_t i=y; i<y+h; i++) {
    setpixel(d, x, i, color);
    setpixel(d, x+w-1, i, color);
  } 
}


// draw a circle
void drawcircle(struct drawdata *d,
	      uint8_t x0, uint8_t y0, uint8_t r, 
	      uint8_t color) {
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  setpixel(d, x0, y0+r, color);
  setpixel(d, x0, y0-r, color);
  setpixel(d, x0+r, y0, color);
  setpixel(d, x0-r, y0, color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    setpixel(d, x0 + x, y0 + y, color);
    setpixel(d, x0 - x, y0 + y, color);
    setpixel(d, x0 + x, y0 - y, color);
    setpixel(d, x0 - x, y0 - y, color);
    
    setpixel(d, x0 + y, y0 + x, color);
    setpixel(d, x0 - y, y0 + x, color);
    setpixel(d, x0 + y, y0 - x, color);
    setpixel(d, x0 - y, y0 - x, color);
    
  }
}


// draw a circle
void fillcircle(struct drawdata *d,
	      uint8_t x0, uint8_t y0, uint8_t r, 
	      uint8_t color) {
  int8_t f = 1 - r;
  int8_t ddF_x = 1;
  int8_t ddF_y = -2 * r;
  int8_t x = 0;
  int8_t y = r;

  for (uint8_t i=y0-r; i<=y0+r; i++) {
    setpixel(d, x0, i, color);
  }

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
  
    for (uint8_t i=y0-y; i<=y0+y; i++) {
      setpixel(d, x0+x, i, color);
      setpixel(d, x0-x, i, color);
    } 
    for (uint8_t i=y0-x; i<=y0+x; i++) {
      setpixel(d, x0+y, i, color);
      setpixel(d, x0-y, i, color);
    }    
  }
}

