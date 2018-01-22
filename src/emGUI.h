#ifndef EMGUI_ARDUINO_H
#define EMGUI_ARDUINO_H

#ifdef __cplusplus
#include <Adafruit_GFX.h> //to add GFX to arduino's include path
#endif

//Wrapper for passing PROGMEM placed strings to const char* methods of emGUI
#define F_WRP(const_char_p) (String(F(const_char_p)).c_str()) 

#include "emGUI/emGUI.hpp"

#endif