#include <avr/io.h>
#include "dio.h"

void Dio_Init(void)
{
  DDRD = DDRD & ~(1 << BUTTON_LH_PIN);    // Make button pins input
  DDRD = DDRD & ~(1 << BUTTON_LL_PIN);
  PORTD = PORTD | (1 << BUTTON_LH_PIN);   // Activate pull up resistance
  PORTD = PORTD | (1 << BUTTON_LL_PIN);

  DDRB = DDRB | (1 << LED_HOT_PIN);       // Make LED pins output
  DDRB = DDRB | (1 << LED_COLD_PIN);
  PORTB = PORTB & ~(1 << LED_HOT_PIN);    // Switch OFF LEDs
  PORTB = PORTB & ~(1 << LED_COLD_PIN);
}

unsigned char Dio_ButtonPressed(unsigned char pin)
{
  if (((PIND >> pin) & 1) == 0)   // a pressed button reads 0
  {
    return 1;
  }
  return 0;
}

void Dio_LedOn(unsigned char pin)
{
  PORTB = PORTB | (1 << pin);
}

void Dio_LedOff(unsigned char pin)
{
  PORTB = PORTB & ~(1 << pin);
}
