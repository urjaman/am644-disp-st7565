// general purpose stuff, use as you'd like!

#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "util.h"

// Some basic delays...
void delay_10us(uint8_t ns)
{
  uint8_t i;

  while (ns != 0) {
    ns--;
    for (i=0; i< 30; i++) {
      nop;
    }
  }
}

void delay_s(uint8_t s) {
  while (s--) {
    _delay_ms(1000);
  }
}

// Some uart functions for debugging help
int uart_putchar(char c)
{
  return 0;
}

char uart_getchar(void) {
  return 0;
}

char uart_getch(void) {
  return 0;
}

void ROM_putstring(const char *str, uint8_t nl) {
}

void uart_puts(const char* str)
{
}


void uart_putc_hex(uint8_t b)
{
}

void uart_putw_hex(uint16_t w)
{
}

void uart_putdw_hex(uint32_t dw)
{
}

void uart_putw_dec(uint16_t w)
{
}

void uart_put_dec(int8_t w)
{
}

void uart_putdw_dec(uint32_t dw)
{
}
