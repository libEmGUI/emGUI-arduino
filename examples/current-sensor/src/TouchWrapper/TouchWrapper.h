#ifndef _TOUCH_WRAPPER_
#define _TOUCH_WRAPPER_

#include <XPT2046_Touchscreen.h>

#include "../../emGUIGlue.h"
#include "../../User_Setup.h"

class TouchWrapper : XPT2046_Touchscreen {
public:
	TouchWrapper() : XPT2046_Touchscreen(TOUCH_CS) {}

	void init(bool debug = false);
	void touchIntr();
	void printTouchRegs();
	bool handleTouch(bool flag);

protected: 
	volatile xTouchEvent currentTouch;
	volatile bool bTouchInt = false;
};
#endif //_TOUCH_WRAPPER_
