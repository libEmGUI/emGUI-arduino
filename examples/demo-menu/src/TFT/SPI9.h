// SPI9.h

// preliminary version 27.12.2015, for ESP8266 only, for MSBFIRST only, not optimized.

// My additions for SPI9 is "Help Yourself Software", no warranty, no support, no obligations.

// JZ 27.12.2016

#ifndef _SPI9_H_
#define _SPI9_H_

#include <SPI.h>

class SPI9Class : public SPIClass
{
  public:
    SPI9Class(uint8_t dcPin);
    uint8_t transfer(uint8_t data);
    void write(uint8_t data);
    void write16(uint16_t data);
    void write32(uint32_t data);
    void writeBytes(uint8_t * data, uint32_t size);
    void writePattern(uint8_t * data, uint8_t size, uint32_t repeat);
    void transferBytes(uint8_t * out, uint8_t * in, uint32_t size);
  private:
    uint8_t _dc;
    inline void setDataBits(uint16_t bits);
};

extern SPI9Class SPI9; // create instance in application, where dcPin to use is known

#endif
