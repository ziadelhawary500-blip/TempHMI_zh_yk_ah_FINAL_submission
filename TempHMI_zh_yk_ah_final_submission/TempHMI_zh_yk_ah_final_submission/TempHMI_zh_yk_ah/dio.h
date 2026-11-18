#ifndef __DIO__
#define __DIO__

#define BUTTON_LH_PIN   2    // PD2, Arduino pin 2
#define BUTTON_LL_PIN   3    // PD3, Arduino pin 3
#define LED_HOT_PIN     0    // PB0, Arduino pin 8 (red)
#define LED_COLD_PIN    1    // PB1, Arduino pin 9 (blue)

void Dio_Init(void);

unsigned char Dio_ButtonPressed(unsigned char pin);

void Dio_LedOn(unsigned char pin);

void Dio_LedOff(unsigned char pin);

#endif
