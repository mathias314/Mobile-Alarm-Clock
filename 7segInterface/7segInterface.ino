// lab2b.c
// Mathew Clutter
// AVR ATMega 2560 @16MHz
// 7 segment display interfacing
// displays 0-9 on the 7 seg display

// connect the segments to PORTA in order (segment A to pin 0, etc)

#include <avr/io.h>
#include <util/delay.h>

int main()
{
  DDRA = 0xFF;
  PORTA = 0x00;

  DDRH = 0x00; 
  // PORTH = 0x00;

  unsigned char displayArray[10];
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

  while(1)
  {
    for(int i = 0; i < 10; i++)
    {
      PORTA = ~displayArray[i];
      _delay_ms(1000); 
    }
  }
}
