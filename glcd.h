#define LCDWIDTH 128
#define LCDHEIGHT 64

#include <alloca.h>
#include <string.h>
#include "stlcdnr.h"

#define swap(a, b) { uint8_t t = a; a = b; b = t; }

struct drawdata {
	uint8_t w;
	uint8_t h;
	uint8_t d[];
};

#define make_drawdata(d,e,j) do { \
				const int asz = sizeof(struct drawdata)+((e)*(j)*LCD_CHARW); \
				d = alloca(asz); \
				memset(d,0,asz); \
				d->w = e; \
				d->h = j; \
			     } while(0)

void setpixel(struct drawdata *d, uint8_t x, uint8_t y, uint8_t color);

void drawrect(struct drawdata *d,
	      uint8_t x, uint8_t y, uint8_t w, uint8_t h, 
	      uint8_t color);

void drawline(struct drawdata *d,
	      uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, 
	      uint8_t color);

void drawcircle(struct drawdata *d, uint8_t x0, uint8_t y0, uint8_t r,uint8_t color);

void fillrect(struct drawdata *d,uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);

