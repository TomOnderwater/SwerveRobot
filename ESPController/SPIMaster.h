#ifndef SPIMaster_h
#define SPIMaster_h

#include "Arduino.h"
#include <SPI.h>

#ifdef ESP32
class SPIMaster
{
  public:
    SPIMaster();
    void sendData(uint8_t * data, size_t len, uint8_t cs);
    void readData(uint8_t * buff, uint8_t cs);

  private:
    const uint32_t baud = 100000; //100 KHz
    SPIClass * spi = NULL;
};

#endif

#ifdef ESP8266

class SPIMaster
{
  public:
    SPIMaster();
    void sendData(uint8_t * data, size_t len, uint8_t cs);
    void readData(uint8_t * buff, uint8_t cs);

  private:
    const uint32_t baud = 100000; //100 KHz
};

#endif
#endif
