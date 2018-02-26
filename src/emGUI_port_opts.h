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
// i.e. const GFXfont * bigFont[] = { &FreeSans24pt7b , &MyriadPro_Regular24pt8b, NULL };

#ifndef EMGUI_USE_UTF8_FONTS
#define EMGUI_USE_UTF8_FONTS    1
#endif 

#include <gfxfont.h> // if using UTF8 fonts, Adafruit-GFX should be updated to use uint16 char ranges

#if EMGUI_USE_UTF8_FONTS
#define EMGUI_XFONT_TYPE const GFXfont **
#else
#define EMGUI_XFONT_TYPE const GFXfont *
#endif // EMGUI_USE_UTF8_FONTS

#define EMGUI_XPICTURE_TYPE  const char*

// STATUSBAR STORY

#define EMGUI_COLOR_MENU_HEADER_TEXT EMGUI_COLOR_BLACK

#define EMGUI_COLOR_STATUS_BAR_BG EMGUI_COLOR_WHITE 


#define EMGUI_COLOR_SW_BLUE 0x4C59


#endif // !ARDIUNO_OPTS_H
