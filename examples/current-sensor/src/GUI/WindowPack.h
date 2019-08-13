#pragma once

//#include "emGUI/emGUI.hpp"
//#include "emGUI/emGUI.h"
#include <emGUI.h>

typedef enum {
	WINDOW_MENU,
	WINDOW_PAINT,
	WINDOW_PLOT
}eWindows;

xFont getBigFont();
xFont getSmallFont();
xFont getBoldFont();

#include "WindowPlot.h"
#include "WindowPaint.h"
#include "WindowMain.h"