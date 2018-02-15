#ifndef EMGUI_PORT_OPTS_H
#define EMGUI_PORT_OPTS_H


#include <stdint.h>

#ifdef __AVR__
 #include <avr/io.h>
 #include <avr/pgmspace.h>
#elif defined(ESP8266)
 #include <pgmspace.h>
#else
 #define PROGMEM
#endif

// if you set this options, all fonts should be returned as null-terminated array!
// i.e. {font_region_1, font_region_2, NULL}

#ifndef EMGUI_USE_UTF8_FONTS
#define EMGUI_USE_UTF8_FONTS    0   
#endif 

#include <gfxfont.h> // if using UTF8 fonts, Adafruit-GFX should be updated to use uint16 char ranges

#define EMGUI_XFONT_TYPE const GFXfont *
#define EMGUI_XPICTURE_TYPE  const char*

#endif // !ARDIUNO_OPTS_H
