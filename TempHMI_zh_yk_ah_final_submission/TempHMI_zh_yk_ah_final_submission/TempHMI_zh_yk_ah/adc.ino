#include <avr/io.h>
#include "adc.h"

void Adc_Init(void)
{
  ADMUX = (1 << REFS1) | (1 << REFS0);                                  // use the internal 1.1 V reference
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // turn ADC on, clock divided by 128
}

unsigned short Adc_ReadChannel(unsigned char channel)
{
  ADMUX = (ADMUX & 0xF0) | channel;     // choose the pin
  ADCSRA = ADCSRA | (1 << ADSC);        // start the conversion

  while (((ADCSRA >> ADSC) & 1) != 0)   // wait until it finishes
  {
  }

  return ADC;   // 0 to 1023
}
