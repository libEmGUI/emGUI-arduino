#ifndef _TOUCH_WRAPPER_
#define _TOUCH_WRAPPER_

#include "emGUIGlue.h"
// FT6206 REGS AND CONSTANTS
#define FT6206_G_MODE        0xA4
#define FT6206_PWR_MODE      0xA5
#define FT6206_CTRL          0x86
#define FT6206_GEST_ID       0x01
#define FT6206_TD_STATUS     0x02
#define FT6206_PERIODMONITOR 0x89
#define FT6206_PERIODACTIVE  0x88
#define FT6206_P1_YH         0x05
#define FT6206_ID_G_PMODE    FT6206_PWR_MODE
#define FT6206_STATE         0xBC

#define FT6206_INTR_POLL_MODE 0
#define FT6206_INTR_TRIG_MODE 1


#include <XPT2046_Touchscreen.h>
#include <Arduino.h>
#include "User_Setup.h"

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
