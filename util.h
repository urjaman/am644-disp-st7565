#include <avr/pgmspace.h>

void uart_putc_hex(uint8_t b);
void uart_putw_hex(uint16_t w);
void uart_putdw_hex(uint32_t dw);

void uart_putw_dec(uint16_t w);
void uart_putdw_dec(uint32_t dw);
void uart_puts(const char* str);

#define nop asm volatile ("nop\n\t")

// some timing functions

void delay_ms(unsigned char ms);
void delay_10us(uint8_t us);
void delay_s(uint8_t s);

