#ifndef __LCD_FINAL__
#define __LCD_FINAL__

// LCD board: SDA goes to A4 and SCL goes to A5

void Lcd_Init(void);

void Lcd_GoTo(unsigned char row, unsigned char col);

void Lcd_PrintString(const char text[]);

void Lcd_PrintNumber(unsigned short value, unsigned char width);

#endif
