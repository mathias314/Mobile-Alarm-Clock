#include <avr/io.h>
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

int main()
{
  Serial.begin(9600);
  remoteInit();
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

ISR
