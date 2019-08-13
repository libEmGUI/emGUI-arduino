#pragma once

#include <XPT2046_Touchscreen.h>
#include "emGUI.h"
#include "../../User_Setup.h"

class TouchWrapper : XPT2046_Touchscreen {
public:
	TouchWrapper() : XPT2046_Touchscreen(TOUCH_CS) {}

	void init(bool debug = false);
	bool handleTouch(bool flag);

protected: 
	volatile xTouchEvent currentTouch;
	volatile bool bTouchInt = false;
	bool alreadyPoped = true;
	bool lastTouched = false;
};
