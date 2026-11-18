TEMPERATURE HMI WITH ATMEGA328P

By Yahia Khalaf, Ziad Ahmed and Amr Khaled.

Our project for Embedded Systems. It reads the temperature from an LM35 and shows it on a 16x2 LCD. Two buttons set the high and low limits, and two LEDs light up when the temperature goes outside them. The code is written in C using registers, no Arduino functions or libraries.


HOW IT WORKS

The LCD shows the temperature on the top line and the two limits on the bottom line.  
Tap a button to raise its limit, hold it to lower it.  
Red LED means too hot, blue LED means too cold.  
If the sensor has a problem, both LEDs turn on.


WIRING

LCD: VCC to 5V, GND to GND, SDA to A4, SCL to A5.  
LM35 (flat side facing you): left leg to 5V, middle leg to A0, right leg to GND.  
LH button: pin 2 and GND.  
LL button: pin 3 and GND.  
Red LED: pin 8 through a 220 ohm resistor, other leg to GND.  
Blue LED: pin 9 through a 220 ohm resistor, other leg to GND.


FILES

TempHMI_zh_yk_ah.ino is the main program.  
adc.h and adc.ino read the sensor.  
lcd_final.h and lcd_final.ino control the LCD.  
dio.h and dio.ino handle the buttons and LEDs.  
tests/TempHMI_Robot_Tests.txt has the test cases.  
bom/BOM.csv and bom/Optimized_BOM.csv are the parts lists.  
REPORT.md is the design document.


UPLOADING

Open TempHMI_zh_yk_ah.ino in the Arduino IDE, pick Arduino Uno and the right port, then upload.


NOTES

If the LCD shows white boxes, adjust the screw on the back.  
If the LM35 gets hot, disconnect it.
