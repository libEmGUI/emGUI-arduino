#pragma once

#include <stdint.h>

class I2CBitbanger {
public:
	I2CBitbanger(uint8_t sda, uint8_t sck);
};