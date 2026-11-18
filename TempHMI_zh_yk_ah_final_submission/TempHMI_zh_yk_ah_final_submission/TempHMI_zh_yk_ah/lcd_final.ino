#include <avr/io.h>
#include <util/delay.h>
#include "lcd_final.h"

unsigned char lcd_address = 0x27;

unsigned char I2c_Wait(void)
{
  unsigned short loops = 0;

  while (((TWCR >> TWINT) & 1) == 0)        // wait, but give up if the LCD is not connected
  {
    loops = loops + 1;
    if (loops > 20000)
    {
      return 0;
    }
  }
  return 1;
}

unsigned char I2c_Start(unsigned char address)
{
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);   // start
  if (I2c_Wait() == 0)
  {
    return 0;
  }
  TWDR = address << 1;                                // address of the LCD board
  TWCR = (1 << TWINT) | (1 << TWEN);
  if (I2c_Wait() == 0)
  {
    return 0;
  }
  return ((TWSR & 0xF8) == 0x18);                     // 1 if the LCD answered
}

void I2c_Stop(void)
{
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  _delay_us(50);
}

void I2c_Send(unsigned char data)
{
  if (I2c_Start(lcd_address) == 1)
  {
    TWDR = data | 0x08;                     // data, with the backlight on
    TWCR = (1 << TWINT) | (1 << TWEN);
    I2c_Wait();
  }
  I2c_Stop();
}

void Lcd_SendNibble(unsigned char nibble, unsigned char rs)
{
  I2c_Send((nibble << 4) | rs | 0x04);      // pulse E so the LCD reads the data
  I2c_Send((nibble << 4) | rs);
  _delay_us(100);
}

void Lcd_Send(unsigned char value, unsigned char rs)
{
  Lcd_SendNibble(value >> 4, rs);           // top half first, then bottom half
  Lcd_SendNibble(value & 0x0F, rs);
}

void Lcd_Init(void)
{
  unsigned char a;

  PORTC = PORTC | (1 << 4) | (1 << 5);      // pull ups on A4 and A5
  TWBR = 72;                                // 100 kHz I2C speed
  TWCR = (1 << TWEN);                       // turn I2C on
  _delay_ms(50);

  for (a = 1; a < 127; a++)                 // find the address of the LCD board
  {
    if (I2c_Start(a) == 1)
    {
      lcd_address = a;
    }
    I2c_Stop();
  }

  Lcd_SendNibble(0x03, 0);                  // start-up steps to put the LCD in 4-bit mode
  _delay_ms(5);
  Lcd_SendNibble(0x03, 0);
  _delay_ms(1);
  Lcd_SendNibble(0x03, 0);
  Lcd_SendNibble(0x02, 0);

  Lcd_Send(0x28, 0);                        // 4-bit mode, 2 lines
  Lcd_Send(0x0C, 0);                        // display on, cursor off
  Lcd_Send(0x06, 0);                        // move right after each character
  Lcd_Send(0x01, 0);                        // clear the screen
  _delay_ms(2);
}

void Lcd_GoTo(unsigned char row, unsigned char col)
{
  if (row == 0)
  {
    Lcd_Send(0x80 + col, 0);
  }
  else
  {
    Lcd_Send(0xC0 + col, 0);                // the bottom row starts at 0x40
  }
}

void Lcd_PrintString(const char text[])
{
  unsigned char i = 0;

  while (text[i] != '\0')
  {
    Lcd_Send(text[i], 1);
    i = i + 1;
  }
}

void Lcd_PrintNumber(unsigned short value, unsigned char width)
{
  char digits[5];
  unsigned char count = 0;

  do                                        // take the digits out from right to left
  {
    digits[count] = '0' + (value % 10);
    value = value / 10;
    count = count + 1;
  } while (value > 0);

  while (width > count)                     // spaces in front so the number lines up
  {
    Lcd_Send(' ', 1);
    width = width - 1;
  }

  while (count > 0)                         // print the digits in the right order
  {
    count = count - 1;
    Lcd_Send(digits[count], 1);
  }
}
