#include <avr/io.h>
#include <util/delay.h>

#define DS1307_READ_ADDR 0xD1
#define DS1307_WRITE_ADDR 0xD0

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

int main()
{
  displayInit();
  TWI_init();
  RTC_init();
  // RTC_setTime(17, 9); // set time before uploading!
  int currTime;
  Serial.begin(9600);

  while(1)
  {
    currTime = RTC_getTime();
    writeNum(currTime);
    Serial.println(currTime);
  }

  return -1;
}
