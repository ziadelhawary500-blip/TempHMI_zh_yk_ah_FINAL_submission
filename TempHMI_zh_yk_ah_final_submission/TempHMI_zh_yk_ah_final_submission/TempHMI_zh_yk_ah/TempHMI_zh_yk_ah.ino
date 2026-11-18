#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "lcd_final.h"
#include "dio.h"

#define TEMP_CHANNEL   0     // LM35 on A0
#define LIMIT_MAX      99
#define HYSTERESIS     5     // 0.5 C (the temperature is kept in tenths)

#define STATUS_OK      0
#define STATUS_HIGH    1
#define STATUS_LOW     2
#define STATUS_ERROR   3

unsigned char low_limit = 20;
unsigned char high_limit = 30;
unsigned char status = STATUS_OK;
unsigned char was_pressed[2] = {0, 0};   // [0] is the LH button, [1] is the LL button
unsigned char hold_time[2] = {0, 0};     // how long each button has been held

unsigned short ReadTemperature(void)
{
  unsigned long sum = 0;
  unsigned char i;

  for (i = 0; i < 8; i++)          // average of 8 readings
  {
    sum = sum + Adc_ReadChannel(TEMP_CHANNEL);
  }
  sum = sum / 8;

  if (sum >= 1023)                 // sensor not connected or too hot to measure
  {
    return 0xFFFF;
  }
  return sum * 1100 / 1024;        // LM35 gives 10 mV per degree, so millivolts = tenths of a degree
}

#define ACTION_NONE    0
#define ACTION_UP      1
#define ACTION_DOWN    2
#define HOLD_START     6     // hold for about 0.6 seconds to start going down
#define HOLD_REPEAT    3     // then go down again about every 0.3 seconds

// Tap a button to go up. Hold it to go down.
unsigned char ButtonAction(unsigned char i, unsigned char pin)
{
  unsigned char pressed = Dio_ButtonPressed(pin);
  unsigned char action = ACTION_NONE;

  if (pressed == 1)
  {
    hold_time[i] = hold_time[i] + 1;
    if (hold_time[i] >= HOLD_START + HOLD_REPEAT)   // still held, so repeat
    {
      hold_time[i] = HOLD_START;
    }
    if (hold_time[i] == HOLD_START)                 // held long enough, go down
    {
      action = ACTION_DOWN;
    }
  }
  else
  {
    if ((was_pressed[i] == 1) && (hold_time[i] < HOLD_START))   // let go quickly, so it was a tap
    {
      action = ACTION_UP;
    }
    hold_time[i] = 0;
  }

  was_pressed[i] = pressed;
  return action;
}

void CheckButtons(void)
{
  unsigned char action;

  action = ButtonAction(0, BUTTON_LH_PIN);
  if ((action == ACTION_UP) && (high_limit < LIMIT_MAX))
  {
    high_limit = high_limit + 1;
  }
  if ((action == ACTION_DOWN) && (high_limit > low_limit + 1))   // LH must stay above LL
  {
    high_limit = high_limit - 1;
  }

  action = ButtonAction(1, BUTTON_LL_PIN);
  if ((action == ACTION_UP) && (low_limit < high_limit - 1))     // LL must stay below LH
  {
    low_limit = low_limit + 1;
  }
  if ((action == ACTION_DOWN) && (low_limit > 0))
  {
    low_limit = low_limit - 1;
  }
}

void CheckTemperature(unsigned short temp)
{
  unsigned short high = high_limit * 10;
  unsigned short low = low_limit * 10;

  if (temp == 0xFFFF)
  {
    status = STATUS_ERROR;
  }
  else if (temp > high)
  {
    status = STATUS_HIGH;
  }
  else if (temp < low)
  {
    status = STATUS_LOW;
  }
  else if ((status == STATUS_HIGH) && (temp > high - HYSTERESIS))
  {
    status = STATUS_HIGH;          // stay on until 0.5 C below LH so the LED doesn't flicker
  }
  else if ((status == STATUS_LOW) && (temp < low + HYSTERESIS))
  {
    status = STATUS_LOW;           // same for LL
  }
  else
  {
    status = STATUS_OK;
  }
}

void UpdateLeds(void)
{
  if ((status == STATUS_HIGH) || (status == STATUS_ERROR))
  {
    Dio_LedOn(LED_HOT_PIN);
  }
  else
  {
    Dio_LedOff(LED_HOT_PIN);
  }

  if ((status == STATUS_LOW) || (status == STATUS_ERROR))
  {
    Dio_LedOn(LED_COLD_PIN);
  }
  else
  {
    Dio_LedOff(LED_COLD_PIN);
  }
}

void ShowScreen(unsigned short temp)
{
  Lcd_GoTo(0, 0);
  if (status == STATUS_ERROR)
  {
    Lcd_PrintString("Sensor ERROR!   ");
  }
  else
  {
    Lcd_PrintString("Temp:");
    Lcd_PrintNumber(temp / 10, 3);   // whole degrees
    Lcd_PrintString(".");
    Lcd_PrintNumber(temp % 10, 1);   // tenths

    if (status == STATUS_HIGH)
    {
      Lcd_PrintString("C HIGH");
    }
    else if (status == STATUS_LOW)
    {
      Lcd_PrintString("C LOW ");
    }
    else
    {
      Lcd_PrintString("C OK  ");
    }
  }

  Lcd_GoTo(1, 0);
  Lcd_PrintString("LL:");
  Lcd_PrintNumber(low_limit, 2);
  Lcd_PrintString("C   LH:");
  Lcd_PrintNumber(high_limit, 2);
  Lcd_PrintString("C ");
}

int main(void)
{
  unsigned short temp;

  Dio_Init();
  Lcd_Init();
  Adc_Init();

  while (1)
  {
    CheckButtons();
    temp = ReadTemperature();
    CheckTemperature(temp);
    UpdateLeds();
    ShowScreen(temp);
    _delay_ms(20);
  }

  return 0;
}
