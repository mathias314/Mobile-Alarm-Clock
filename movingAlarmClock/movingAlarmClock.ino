#include <avr/io.h>
#include <util/delay.h>
#include <HCSR04.h>
#include "IRremote.h"
#include <avr/interrupt.h>

#define DS1307_READ_ADDR 0xD1
#define DS1307_WRITE_ADDR 0xD0

#define TRIG_PIN 15
#define ECHO_PIN 16

int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11

bool active = false;

UltraSonicDistanceSensor distanceSensor(TRIG_PIN, ECHO_PIN);
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;

void writeNum(unsigned int);

unsigned char displayArray[10];

void TWI_init()
{
  TWSR = 0x00;
  TWBR = 0x98;
  TWCR = (1 << TWEN);
}

void TWI_start()
{
  TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
  while (!(TWCR & (1<<TWINT)));
}

void TWI_write(unsigned char data)
{
  TWDR = data;
  TWCR = ((1 << TWINT) | (1 << TWEN));
  while(!(TWCR & (1 << TWINT)));
}

unsigned char TWI_read(unsigned char ackVal)
{
  TWCR = ((1<<TWINT | (1<<TWEN) | (ackVal<<TWEA)));
  while(!(TWCR & (1<<TWINT)));
  return TWDR;
}

void TWI_stop()
{
  TWCR = ((1<<TWINT) | (1<<TWEN) | (1<<TWSTO));
  _delay_us(100);
}

void RTC_init()
{
  TWI_init();
  TWI_start();
  TWI_write(DS1307_WRITE_ADDR);
  TWI_write(0x07);
  TWI_write(0x00);
  TWI_stop();
}

void RTC_setDate(unsigned char year, unsigned char mon, unsigned char day)
{
  TWI_start();
  TWI_write(DS1307_WRITE_ADDR);
  TWI_write(day);
  TWI_write(mon);
  TWI_write(year);
  TWI_stop();
}

void RTC_setTime(unsigned char hour, unsigned char min)
{
  TWI_start();
  TWI_write(DS1307_WRITE_ADDR);
  TWI_write(0x01);
  TWI_write(min);
  TWI_write(hour);
  TWI_stop();
}

int RTC_getTime()
{
  TWI_start();
  TWI_write(DS1307_WRITE_ADDR);
  TWI_write(0x01);
  TWI_stop();

  TWI_start();
  TWI_write(DS1307_READ_ADDR);

  int min = TWI_read(1);
  int hour = TWI_read(0);
  // Serial.println(hour);
  // Serial.println(min);
  // Serial.println();

  TWI_stop();

  return int((hour * 100) + min);
}

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

void displayInit()
{
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

void ultrasonicInit()
{
  sei();
}

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
  displayInit();
  TWI_init();
  RTC_init();
  ultrasonicInit();
  remoteInit();
  buttonInit();
  
  // RTC_setTime(21, 35); // set time before uploading!
  
  int currTime;
  int dist = 0;
  Serial.begin(9600);
  _delay_ms(1000);

  while(1)
  {
    Serial.println(active);
    if(irrecv.decode(&results))
      {
        if(results.value == 0xFF02FD)
        {
          // Serial.println("start!");
          active = true;
        }
        irrecv.resume();
      }
    
    currTime = RTC_getTime();
    writeNum(currTime);
    dist = int(distanceSensor.measureDistanceCm());
    if(dist < 0)
    {
      dist = 400;
    }
    // Serial.println(dist);
  }

  return -1;
}


ISR(INT4_vect)
{
  // Serial.println("button pressed");
  active = false;
}
