#pragma once

#if defined(__cplusplus) && !defined(_MSC_VER)
//#include <Adafruit_GFX.h> //to add GFX to arduino's include path
#endif

#if !defined(_MSC_VER)
//Wrapper for passing PROGMEM placed strings to const char* methods of emGUI
#define F_WRP(const_char_p) (String(F(const_char_p)).c_str()) 
#endif

#if __cplusplus >= 201103L
#include "emGUI/emGUI.hpp"
#endif
