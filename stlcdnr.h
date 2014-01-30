/* This is the external API */
#define LCD_MAXX 16
#define LCD_MAXY 8
#define LCD_CHARW 8
#define LCD_CHARH 8

void lcd_init(void);
void lcd_putchar(unsigned char c);
void lcd_puts(const unsigned char* str);
void lcd_puts_P(PGM_P str);
void lcd_clear(void);
void lcd_gotoxy(uint8_t x, uint8_t y);

/* Buffer should be w*h*LCD_CHARW bytes big. */
void lcd_write_block_P(const PGM_P buffer, uint8_t w, uint8_t h);
void lcd_write_block(const uint8_t* buffer, uint8_t w, uint8_t h);

void st7565_set_contrast(uint8_t val);

/* End header */
