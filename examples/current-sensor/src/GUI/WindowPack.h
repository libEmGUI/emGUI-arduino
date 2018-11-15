#ifndef _WINDOW_PACK_
#define _WINDOW_PACK_


//#include "emGUI/emGUI.hpp"
//#include "emGUI/emGUI.h"
#include <emGUI.h>
#include "../../emGUIGlue.h"


typedef enum {
	WINDOW_MENU,
	WINDOW_TP_TEST,
	WINDOW_PLOT
}eWindows;

#define COLOR_BLUE EMGUI_COLOR_SEA_3
#define so 5
#define bo 14

xFont getBigFont();
xFont getSmallFont();
xFont getBoldFont();

// #include "TimeClock.hpp"
#include "ButtonWithBg.hpp"
#include "TextFrame.hpp"
#include "WindowPlot.h"

#endif // !_WINDOW_PACK_