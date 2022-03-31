#include "SPIMaster.h"

#ifdef ESP32
SPIMaster::SPIMaster()
{
  spi = new SPIClass(HSPI);
  spi->begin();
}


void SPIMaster::sendData(uint8_t * data, size_t len, uint8_t cs)
{
  uint8_t i = 0;
  spi->beginTransaction(SPISettings(baud, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  spi->transfer(0x02);
  spi->transfer(0x00);
  while (len-- && i < 32) {
    spi->transfer(data[i++]);
  }
  while (i++ < 32) {
    spi->transfer(0);
  }
  digitalWrite(cs, HIGH);
  spi->endTransaction();
}

void SPIMaster::readData(uint8_t * buff, uint8_t cs)
{
  spi->beginTransaction(SPISettings(baud, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  spi->transfer(0x03);
  spi->transfer(0x00);
  for (uint8_t i = 0; i < 32; i++) {
    buff[i] = spi->transfer(0);
  }
  digitalWrite(cs, HIGH);
  spi->endTransaction();
}
#endif

#ifdef ESP8266
SPIMaster::SPIMaster()
{
  SPI.begin();
}

void SPIMaster::sendData(uint8_t * data, size_t len, uint8_t cs)
{
  uint8_t i = 0;
  SPI.beginTransaction(SPISettings(baud, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  SPI.transfer(0x02);
  SPI.transfer(0x00);
  while (len-- && i < 32) {
    SPI.transfer(data[i++]);
  }
  while (i++ < 32) {
    SPI.transfer(0);
  }
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}

void SPIMaster::readData(uint8_t * buff, uint8_t cs)
{
  SPI.beginTransaction(SPISettings(baud, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  SPI.transfer(0x03);
  SPI.transfer(0x00);
  for (uint8_t i = 0; i < 32; i++) {
    buff[i] = SPI.transfer(0);
  }
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}
#endif
