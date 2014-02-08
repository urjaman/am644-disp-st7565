#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "glcd.h"


void draw_drawdata(struct drawdata *dd) {
    for (int y=0;y<dd->h;y++) {
        for (int x=0;x<dd->w;x++) {
            printf("%c",(dd->d[x+(y/8)*dd->w] & (1<<(7-(y&7))))?'O':' ');
        }
        printf("\n");
     } 
}

int main(void) {
    struct drawdata *dd;
    make_drawdata(dd,LCD_MAXX,LCD_MAXY);
    uint8_t xc = dd->w/2;
    uint8_t yc = dd->h/2;
    fillrect(dd,1,1,6,6,1);
    fillrect(dd,120,0,8,8,1);
    fillrect(dd,121,1,6,6,0);
    drawline(dd,7,9,1,63,1);
    drawline(dd,1,63,127,57,1);
    drawline(dd,1,57,127,63,1);
    drawline(dd,121,9,127,63,1);
    fillcircle(dd,xc,yc,3,1);
    drawcircle(dd,xc,yc,5,1);
    drawcircle(dd,xc,yc,8,1);
    drawcircle(dd,xc,yc,11,1);
    drawcircle(dd,xc,yc,14,1);
    drawcircle(dd,xc,yc,17,1);
    drawcircle(dd,xc,yc,20,1);
    draw_drawdata(dd);
    return 0;
}   
