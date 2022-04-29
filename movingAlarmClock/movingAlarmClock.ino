//www.elegoo.com
//2018.10.24
#include <Wire.h>
#include <DS3231.h>
#include <avr/io.h>
#include <util/delay.h>

DS3231 clock;
RTCDateTime dt;

void writeNum(unsigned int);

unsigned char displayArray[10];

void writeNum(unsigned int num)
{
  if(num > 9999)
  {
    return;
  }

  int currDig;
  
  currDig = num % 10;
  num = num / 10;
  PORTC = 0x01;
  PORTA = ~displayArray[currDig];
  _delay_ms(1);

  currDig = num % 10;
  num = num / 10;
  PORTC = 0x02;
  PORTA = ~displayArray[currDig];
  _delay_ms(1);

  currDig = num % 10;
  num = num / 10;
  PORTC = 0x04;
  PORTA = ~displayArray[currDig];
  _delay_ms(1);

  currDig = num % 10;
  num = num / 10;
  PORTC = 0x08;
  PORTA = ~displayArray[currDig];
  _delay_ms(1);

  return;
}

void setup()
{
  Serial.begin(9600);

  Serial.println("Initialize RTC module");
  // Initialize DS3231
  clock.begin();

  
  // Manual (YYYY, MM, DD, HH, II, SS
  // clock.setDateTime(2022, 4, 28, 8, 12, 35);
  
  // Send sketch compiling time to Arduino
  clock.setDateTime(__DATE__, __TIME__);    
  /*
  Tips:This command will be executed every time when Arduino restarts. 
       Comment this line out to store the memory of DS3231 module
  */

  displayArray[0] = 0b00111111;
  displayArray[1] = 0b00000110;
  displayArray[2] = 0b01011011;
  displayArray[3] = 0b01001111;
  displayArray[4] = 0b01100110;
  displayArray[5] = 0b01101101;
  displayArray[6] = 0b01111101;
  displayArray[7] = 0b00000111;
  displayArray[8] = 0b01111111;
  displayArray[9] = 0b01101111;

  DDRA = 0xFF;
  PORTA = 0x00;

  DDRC = 0xFF; 
  PORTC = 0xFF;
}

void loop()
{
  dt = clock.getDateTime();

  // For leading zero look to DS3231_dateformat example

  
  Serial.print(dt.hour);   
  Serial.print(":");
  Serial.print(dt.minute);
  Serial.println();

  writeNum(dt.second);
  
  // delay(1000);
}
