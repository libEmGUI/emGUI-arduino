#pragma once


//#include "emGUI/emGUI.hpp"
//#include "emGUI/emGUI.h"
#include <emGUI.h>
#include "../../emGUIGlue.h"


typedef enum {
	WINDOW_MENU,
	WINDOW_PAINT,
	WINDOW_PLOT
}eWindows;

xFont getBigFont();
xFont getSmallFont();
xFont getBoldFont();

// #include "TimeClock.hpp"
#include "WindowPlot.h"
#include "WindowPaint.h"
#include "WindowMain.h"