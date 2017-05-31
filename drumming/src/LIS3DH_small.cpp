/**************************************************************************/
/*!
    @file     Adafruit_LIS3DH.cpp
    @author   K. Townsend / Limor Fried (Adafruit Industries)
    @license  BSD (see license.txt)

    This is a library for the Adafruit LIS3DH Accel breakout board
    ----> https://www.adafruit.com/products/2809

    Adafruit invests time and resources providing this open source code,
    please support Adafruit and open-source hardware by purchasing
    products from Adafruit!

    @section  HISTORY

    v1.0  - First release
*/
/**************************************************************************/

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <TinyWireM.h>
#include "LIS3DH_small.h"


/**************************************************************************/
/*!
    @brief  Instantiates a new LIS3DH class in I2C or SPI mode
*/
/**************************************************************************/
// I2C
Adafruit_LIS3DH_small::Adafruit_LIS3DH_small()
  : _cs(-1), _mosi(-1), _miso(-1), _sck(-1), _sensorID(-1)
{
}

Adafruit_LIS3DH_small::Adafruit_LIS3DH_small(int8_t cspin)
  : _cs(cspin), _mosi(-1), _miso(-1), _sck(-1), _sensorID(-1)
{ }

Adafruit_LIS3DH_small::Adafruit_LIS3DH_small(int8_t cspin, int8_t mosipin, int8_t misopin, int8_t sckpin)
  : _cs(cspin), _mosi(mosipin), _miso(misopin), _sck(sckpin), _sensorID(-1)
{ }



/**************************************************************************/
/*!
    @brief  Setups the HW (reads coefficients values, etc.)
*/
/**************************************************************************/
bool Adafruit_LIS3DH_small::begin(uint8_t i2caddr) {
  _i2caddr = i2caddr;

  TinyWireM.begin();




  /* Check connection */
//  uint8_t deviceid = readRegister8(LIS3DH_REG_WHOAMI);
//  if (deviceid != 0x33)
//  {
//    /* No LIS3DH detected ... return false */
//    //Serial.println(deviceid, HEX);
//    return false;
//  }

  // enable all axes, normal mode
  writeRegister8(LIS3DH_REG_CTRL1, 0x07);
  // 400Hz rate
  setDataRate(LIS3DH_DATARATE_400_HZ);

  // High res & BDU enabled
  writeRegister8(LIS3DH_REG_CTRL4, 0x88);

  // DRDY on INT1
  writeRegister8(LIS3DH_REG_CTRL3, 0x10);

  // Turn on orientation config
  //writeRegister8(LIS3DH_REG_PL_CFG, 0x40);

  // enable adcs
  writeRegister8(LIS3DH_REG_TEMPCFG, 0x80);

  /*
  for (uint8_t i=0; i<0x30; i++) {
    Serial.print("$");
    Serial.print(i, HEX); Serial.print(" = 0x");
    Serial.println(readRegister8(i), HEX);
  }
  */

  return true;
}


void Adafruit_LIS3DH_small::read(void) {
  // read x y z at once

  if (_cs == -1) {
    // i2c
    TinyWireM.beginTransmission(_i2caddr);
    TinyWireM.write(LIS3DH_REG_OUT_X_L | 0x80); // 0x80 for autoincrement
    TinyWireM.endTransmission();

    TinyWireM.requestFrom(_i2caddr, 6);
    x = TinyWireM.read(); x |= ((uint16_t)TinyWireM.read()) << 8;
    y = TinyWireM.read(); y |= ((uint16_t)TinyWireM.read()) << 8;
    z = TinyWireM.read(); z |= ((uint16_t)TinyWireM.read()) << 8;
  }
  #ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(LIS3DH_REG_OUT_X_L | 0x80 | 0x40); // read multiple, bit 7&6 high

    x = spixfer(); x |= ((uint16_t)spixfer()) << 8;
    y = spixfer(); y |= ((uint16_t)spixfer()) << 8;
    z = spixfer(); z |= ((uint16_t)spixfer()) << 8;

    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPI.endTransaction();              // release the SPI bus

  }
  #endif
  uint8_t range = getRange();
  uint16_t divider = 1;
  if (range == LIS3DH_RANGE_16_G) divider = 1365; // different sensitivity at 16g
  if (range == LIS3DH_RANGE_8_G) divider = 4096;
  if (range == LIS3DH_RANGE_4_G) divider = 8190;
  if (range == LIS3DH_RANGE_2_G) divider = 16380;


}

/**************************************************************************/
/*!
    @brief  Read the auxilary ADC
*/
/**************************************************************************/

int16_t Adafruit_LIS3DH_small::readADC(uint8_t adc) {
  return 0;
}


/**************************************************************************/
/*!
    @brief  Set INT to output for single or double click
*/
/**************************************************************************/

void Adafruit_LIS3DH_small::setClick(uint8_t c, uint8_t clickthresh, uint8_t timelimit, uint8_t timelatency, uint8_t timewindow) {

}

uint8_t Adafruit_LIS3DH_small::getClick(void) {
return 0;
}


/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer
*/
/**************************************************************************/
void Adafruit_LIS3DH_small::setRange(lis3dh_range_t range)
{
  uint8_t r = readRegister8(LIS3DH_REG_CTRL4);
  r &= ~(0x30);
  r |= range << 4;
  writeRegister8(LIS3DH_REG_CTRL4, r);
}

/**************************************************************************/
/*!
    @brief  Sets the g range for the accelerometer
*/
/**************************************************************************/
lis3dh_range_t Adafruit_LIS3DH_small::getRange(void)
{
  /* Read the data format register to preserve bits */
  return (lis3dh_range_t)((readRegister8(LIS3DH_REG_CTRL4) >> 4) & 0x03);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the LIS3DH (controls power consumption)
*/
/**************************************************************************/
void Adafruit_LIS3DH_small::setDataRate(lis3dh_dataRate_t dataRate)
{
  uint8_t ctl1 = readRegister8(LIS3DH_REG_CTRL1);
  ctl1 &= ~(0xF0); // mask off bits
  ctl1 |= (dataRate << 4);
  writeRegister8(LIS3DH_REG_CTRL1, ctl1);
}

/**************************************************************************/
/*!
    @brief  Sets the data rate for the LIS3DH (controls power consumption)
*/
/**************************************************************************/
lis3dh_dataRate_t Adafruit_LIS3DH_small::getDataRate(void)
{
  return (lis3dh_dataRate_t)((readRegister8(LIS3DH_REG_CTRL1) >> 4)& 0x0F);
}

/**************************************************************************/
/*!
    @brief  Gets the most recent sensor event
*/
/**************************************************************************/
bool Adafruit_LIS3DH_small::getEvent(sensors_event_t *event) {
/*
//
  memset(event, 0, sizeof(sensors_event_t));

  event->version   = sizeof(sensors_event_t);
  event->sensor_id = _sensorID;
  event->type      = SENSOR_TYPE_ACCELEROMETER;
  event->timestamp = 0;

  read();

  event->acceleration.x = x_g * SENSORS_GRAVITY_STANDARD;
  event->acceleration.y = y_g * SENSORS_GRAVITY_STANDARD;
  event->acceleration.z = z_g * SENSORS_GRAVITY_STANDARD;
*/
}

/**************************************************************************/
/*!
    @brief  Gets the sensor_t data
*/
/**************************************************************************/
void Adafruit_LIS3DH_small::getSensor(sensor_t *sensor) {

}


/**************************************************************************/
/*!
    @brief  Low level SPI
*/
/**************************************************************************/

uint8_t Adafruit_LIS3DH_small::spixfer(uint8_t x) {
  #ifndef __AVR_ATtiny85__
  if (_sck == -1)
    return SPI.transfer(x);

  // software spi
  //Serial.println("Software SPI");
  uint8_t reply = 0;
  for (int i=7; i>=0; i--) {
    reply <<= 1;
    digitalWrite(_sck, LOW);
    digitalWrite(_mosi, x & (1<<i));
    digitalWrite(_sck, HIGH);
    if (digitalRead(_miso))
      reply |= 1;
  }
  return reply;
  #endif
}


/**************************************************************************/
/*!
    @brief  Writes 8-bits to the specified destination register
*/
/**************************************************************************/
void Adafruit_LIS3DH_small::writeRegister8(uint8_t reg, uint8_t value) {
  if (_cs == -1) {
    TinyWireM.beginTransmission((uint8_t)_i2caddr);
    TinyWireM.write((uint8_t)reg);
    TinyWireM.write((uint8_t)value);
    TinyWireM.endTransmission();
  }
  #ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(reg & ~0x80); // write, bit 7 low
    spixfer(value);
    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPI.endTransaction();              // release the SPI bus
  }
  #endif
}

/**************************************************************************/
/*!
    @brief  Reads 8-bits from the specified register
*/
/**************************************************************************/
uint8_t Adafruit_LIS3DH_small::readRegister8(uint8_t reg) {
  uint8_t value;

  if (_cs == -1) {
    TinyWireM.beginTransmission(_i2caddr);
    TinyWireM.write((uint8_t)reg);
    TinyWireM.endTransmission();

    TinyWireM.requestFrom(_i2caddr, 1);
    value = TinyWireM.read();
  }
  #ifndef __AVR_ATtiny85__
  else {
    if (_sck == -1)
      SPI.beginTransaction(SPISettings(500000, MSBFIRST, SPI_MODE0));
    digitalWrite(_cs, LOW);
    spixfer(reg | 0x80); // read, bit 7 high
    value = spixfer(0);
    digitalWrite(_cs, HIGH);
    if (_sck == -1)
      SPI.endTransaction();              // release the SPI bus
  }
  #endif
  return value;
}
