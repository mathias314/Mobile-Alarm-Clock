#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "IRremote.h"

int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11

bool active = false;

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;

void remoteInit()
{
  irrecv.enableIRIn();
}

void buttonInit()
{
  DDRE = 0x00;
  sei();
  // set the int masks on PORTD3:0 on rising edge
  EICRB = 0xFF;
  // enable interrupts
  EIMSK |= 0x10;
}

int main()
{
  Serial.begin(9600);
  remoteInit();
  buttonInit();
  while(1)
  {
      if(irrecv.decode(&results))
      {
        if(results.value == 0xFF02FD)
        {
          Serial.println("start!");
          active = true;
        }
        irrecv.resume();
      }
  }
  return -1;
}

ISR(INT4_vect)
{
  Serial.println("button pressed");
  active = false;
}
