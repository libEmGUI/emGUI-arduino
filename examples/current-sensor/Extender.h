#ifndef _EXTENDER_H_
#define _EXTENDER_H_

#include <Wire.h>
#include <Arduino.h>
#include <stdint.h>

#include <memory>

class Extender {
  public:
    typedef std::shared_ptr<Extender> SharedPtr;

    enum {
      IODIR   = 0x00,   // Controls the direction of the data I/O.
      IPOL    = 0x01,   // The IPOL register allows the user to configure the polarity on the corresponding GPIO port bits.
      GPINTEN = 0x02,   // The GPINTEN register controls the interrupt-onchange feature for each pin.
      DEFVAL  = 0x03,
      INTCON  = 0x04,
      IOCON   = 0x05,
      GPPU    = 0x06,
      INTF    = 0x07,
      INTCAP  = 0x08,
      GPIO    = 0x09,
      OLAT    = 0x0A
    } REGs;

    enum {
      IOCON_INTPOL = (1 << 1),
      IOCON_ODR    = (1 << 2),
      IOCON_HAEN   = (1 << 3),
      IOCON_DISSLW = (1 << 4),
      IOCON_SEQOP  = (1 << 5)
    } IOCON_BITS;

    enum {
      ADDR0 = 0x20
    } ADDRs;
    Extender(uint8_t addr = ADDR0): _addr(addr) {
      _gpio = readFromExtender(GPIO);
    }

    void configExtender(uint8_t data1, uint8_t data2);   // write to PE
    uint8_t readFromExtender(int reg);               // read from PE


    void enableIntAsOD();             // Enable INT pin fot open-drain configuration
    void enableSEQOP();               // enable incementing mode for the address pointer
    void configAsInputs(uint8_t pins);     // set given bits as inputs
    void setInternalPU(uint8_t pins);
    void setPins(uint8_t pin, bool enabled);
    void togglePins(uint8_t pin);
    void enableIntForPins(uint8_t pins); // enable interrupt-on-change for corresponding pin

  protected:
    uint8_t _addr;
    uint8_t _gpio;

};

#endif //_EXTENDER_H_
