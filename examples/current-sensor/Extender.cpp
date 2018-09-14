#include "Extender.h"

void Extender::configExtender(uint8_t data1, uint8_t data2) {
  Wire.beginTransmission(_addr);
  Wire.write(data1);
  Wire.write(data2);
  auto status = Wire.endTransmission();
}

void Extender::enableIntAsOD() {
  configExtender(IOCON, IOCON_ODR);
}

void Extender::enableSEQOP() {
  configExtender(IOCON, IOCON_SEQOP);
}

void Extender::configAsInputs(uint8_t pins) {
  configExtender(IODIR, pins);
}

void Extender::setInternalPU(uint8_t pins) {
  configExtender(GPPU, pins);
}

void Extender::enableIntForPins(uint8_t pins) {
  configExtender(GPINTEN, pins);
}

void Extender::setPins(uint8_t pin, bool enabled) {
  _gpio = readFromExtender(GPIO);
  if (enabled) {
    _gpio |= pin;
    configExtender(GPIO, _gpio);
  } else {
    _gpio &= ~pin;
    configExtender(GPIO, _gpio);
  }
}

void Extender::togglePins(uint8_t pin) {
  _gpio ^= pin;
  configExtender(GPIO, _gpio);
}

uint8_t Extender::readFromExtender(int reg) {
  Wire.beginTransmission(_addr);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(_addr, 1);
  auto ret = Wire.read();
  return ret;
}

