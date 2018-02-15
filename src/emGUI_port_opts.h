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

#include <gfxfont.h>

typedef struct {
	GFXfont *regions[];
}UTF8Font;

#define EMGUI_XPICTURE_TYPE  const char*
#define EMGUI_XFONT_TYPE const GFXfont *

#define EM_GUI_PICTURE_STORAGE_ATTR PROGMEM

#endif // !ARDIUNO_OPTS_H
