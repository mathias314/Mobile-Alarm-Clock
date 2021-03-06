// Mathew Clutter and Aaron Quizon
// Embedded Systems Final Project
// Alarm clock that runs away

// 4 digit 7 segment display connected to ports a and c
// each segment is connected to port a,
// with the 'a' segment connected to PA0, the 'b' segment connected to PA1, etc
// the common anode is connected to port c,
// leftmost display connected to PC0, rightmost display connected to PC3

// real time clock (rtc) is connected to the I2C ports (SDA and SCL)

// ultrasonic sensor is connected to pins 15 and 16 (PJ0 and PH1)
// PJ0 connected to ultrasonic trigger pin,
// PH1 connected to ultrasonic echo pin

// IR remote receiever is connected to pin 49 (PL0)

// finally, the dc motors are connected to OC0A (PB7)
// the buzzer is connected to OC0B (PG5)

// this program runs on ATMega2560 at 16MHz

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <HCSR04.h>
#include "IRremote.h"

#define DS1307_READ_ADDR 0xD1
#define DS1307_WRITE_ADDR 0xD0

#define TRIG_PIN 15
#define ECHO_PIN 16

#define receiver 49

volatile bool active = false;

UltraSonicDistanceSensor distanceSensor(TRIG_PIN, ECHO_PIN);
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;

void TWI_init();
void TWI_start();
void TWI_write(unsigned char data);
unsigned char TWI_read(unsigned char ackVal);
void TWI_stop();
void RTC_init();
void RTC_setTime(unsigned char hour, unsigned char min);
int RTC_getTime();
void writeNum(unsigned int num);
void displayInit();
void ultrasonicInit();
void remoteInit();
void buttonInit();
void pwmInit();
void initAll();


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

void pwmInit()
{
  DDRB = 0xFF;
  PORTB = 0xFF;

  TCCR0A = 0xA3;
  TCCR0B = 0x05;

  TCNT0 = 0x00;

  DDRG = 0xFF;
}

void initAll()
{
  displayInit();
  TWI_init();
  RTC_init();
  ultrasonicInit();
  remoteInit();
  buttonInit();
  pwmInit();
}

int main()
{
  initAll(); // call every needed init function
  
  // RTC_setTime(16, 17); // set time before uploading!
  
  int currTime;
  int dist = 0;
  // Serial.begin(9600);
  _delay_ms(500);

  while(1)
  {
    // Serial.println(active);

    currTime = RTC_getTime();
    writeNum(currTime);
    
    if(irrecv.decode(&results))
    {
        if(results.value == 0xFF02FD)
        {
          // Serial.println("start!");
          active = true;
        }
        irrecv.resume();
    }

    if(active)
    {
      dist = int(distanceSensor.measureDistanceCm());
      if(dist < 0)
      {
        dist = 400;
      }
      // Serial.println(dist);
      OCR0A = map(dist, 0, 400, 255, 0); // set motor speed accordingly...
      
      OCR0B = 192;// set buzzer tone
    }
    else
    {
      OCR0A = 0;
      OCR0B = 255;
      // turn motor and buzzer off
    }
  }

  return -1;
}


ISR(INT4_vect)
{
  // Serial.println("button pressed");
  active = false;
}
