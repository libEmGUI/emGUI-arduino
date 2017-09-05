// SPI9.cpp

// My additions for SPI9 is "Help Yourself Software", no warranty, no support, no obligations.

// JZ 27.12.2016

#include "SPI9.h"

SPI9Class::SPI9Class(uint8_t dcPin) : _dc(dcPin)
{
}

inline void SPI9Class::setDataBits(uint16_t bits)
{
  const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
  bits--;
  SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}

uint8_t SPI9Class::transfer(uint8_t data)
{
  while (SPI1CMD & SPIBUSY) {}
  // low byte first, ms bit first!; CMD : dc low -> first bit 0, DATA : dc high -> first bit high
  setDataBits(9);
  SPI1W0 = (data >> 1) | (data << 15) | (digitalRead(_dc) << 7);
  SPI1CMD |= SPIBUSY;
  while (SPI1CMD & SPIBUSY) {}
  uint16_t rv = SPI1W0;
  return uint8_t((rv << 1) | (rv >> 15)); // most likely first try is wrong, need to find test method
}

void SPI9Class::write(uint8_t data)
{
  while (SPI1CMD & SPIBUSY) {}
  // low byte first, ms bit first!; CMD : dc low -> first bit 0, DATA : dc high -> first bit high
  setDataBits(9);
  SPI1W0 = (data >> 1) | (data << 15) | (digitalRead(_dc) << 7);
  SPI1CMD |= SPIBUSY;
  while (SPI1CMD & SPIBUSY) {}
}

void SPI9Class::write16(uint16_t data)
{ // no easy optimization
  write(data >> 8);
  write(data);
}

void SPI9Class::write32(uint32_t data)
{ // no easy optimization
  union
  {
    uint32_t l;
    uint8_t b[4];
  } data_;
  data_.l = data;
  write(data_.b[3]);
  write(data_.b[2]);
  write(data_.b[1]);
  write(data_.b[0]);
}

void SPI9Class::writeBytes(uint8_t * data, uint32_t size)
{ // no easy optimization
  for (uint8_t i = 0; i < size; i++)
  {
    write(data[i]);
  }
  return;
}

void SPI9Class::writePattern(uint8_t * data, uint8_t size, uint32_t repeat)
{ // no easy optimization
  for (uint32_t r = 0; r < repeat; r++)
  {
    for (uint8_t i = 0; i < size; i++)
    {
      write(data[i]);
    }
    yield(); // avoid wdt timeout on low bitrates
  }
}
void SPI9Class::transferBytes(uint8_t * out, uint8_t * in, uint32_t size)
{ // no easy optimization
  for (uint32_t i = 0; i < size; i++)
  {
    out[i] = transfer(in[i]);
  }
  return;
}


