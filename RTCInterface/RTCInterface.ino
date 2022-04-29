#include <avr/io.h>
#include <util/delay.h>

#define DS1307_READ_ADDR 0xD1
#define DS1307_WRITE_ADDR 0xD0

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

int main()
{
  TWI_init();
  RTC_init();
  RTC_setTime(16, 53); // set hour and minute here
  Serial.begin(9600);
  int x;
  
  while(1)
  {
    x = RTC_getTime();
    Serial.println(x);
    _delay_ms(1000);
  }
  
  return -1;
}
