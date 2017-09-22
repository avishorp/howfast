#include "record.h"

#include <Arduino.h>
#include <EEPROM.h>

int read_record() {
  byte b1, b2;
  byte checksum;
  b1 = EEPROM.read(0);
  b2 = EEPROM.read(1);
  checksum = EEPROM.read(2);


#ifdef TRINKET_PRO
  Serial.print(b1); Serial.print(' ');
  Serial.print(b2); Serial.print(' ');
  Serial.print(checksum); Serial.println(' ');
#endif

  if (checksum != (b1 ^ b2))
    return 0;
  else
    return (b1 + (b2 << 8));
}


void write_record(int value) {
  byte b1 = value & 0xff;
  byte b2 = (value >> 8) & 0xff;
  byte checksum = (b1 ^ b2);
  EEPROM.write(0, b1);
  EEPROM.write(1, b2);
  EEPROM.write(2, checksum);
}

void erase_record() {
  EEPROM.write(0, 0xff);
  EEPROM.write(1, 0xff);
  EEPROM.write(2, 0xff);
}
