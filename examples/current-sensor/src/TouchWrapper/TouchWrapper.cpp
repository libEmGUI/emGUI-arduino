#include "TouchWrapper.h"

void TouchWrapper::init(bool debug ) {
	Serial.println("configuring touchouch");
	begin();

	bTouchInt = false;
	if (debug) {
		printTouchRegs();
	}
}

void TouchWrapper::touchIntr() {
	bTouchInt = true;
}

void TouchWrapper::printTouchRegs(){
}


bool TouchWrapper::handleTouch(bool flag){

  static bool alreadyPoped = true;
  static bool lastTouched = false;
#if defined(_MSC_VER)
  static int yCompensationB = 0;
  static int mul = 1000;
  static int yCompensationA = ((EMGUI_LCD_HEIGHT - yCompensationB) * mul) / EMGUI_LCD_HEIGHT;

  static int xCompensationB = 0;
  static int xCompensationA = ((EMGUI_LCD_WIDTH - xCompensationB) * mul) / EMGUI_LCD_WIDTH;
#else

  static int yCompensationB = -20;
  static int mul = 1000;
  static int yCompensationA = ((EMGUI_LCD_HEIGHT - yCompensationB) * mul) / EMGUI_LCD_HEIGHT;

  static int xCompensationB = -10;
  static int xCompensationA = ((EMGUI_LCD_WIDTH - xCompensationB) * mul) / EMGUI_LCD_WIDTH;
#endif

  bool touched1 = false;
  

    currentTouch.eventTime = millis();
    bool touchFlag = touched(); 

    if (touchFlag) {
      alreadyPoped = false;
      uint16_t xptr;
      uint16_t yptr;

      TS_Point p = getPoint();

      currentTouch.x = 320 - ((p.x - 300) * 10 /115);   // * xCompensationA / mul + xCompensationB;
      currentTouch.y = (p.y-300) * 10 / 154;   // * yCompensationA / mul + yCompensationB;
      
      if(!lastTouched){
        currentTouch.event = pushTs;
        Serial.printf(F_WRP("push x: %d; y: %d; m: %d;\n"), currentTouch.x, currentTouch.y, currentTouch.eventTime);
      }
      else{
        currentTouch.event = updateTs;
        Serial.printf(F_WRP("upd x: %d; y: %d; m: %d;\n"), currentTouch.x, currentTouch.y, currentTouch.eventTime);
      }
      bWindowManagerCheckTouchScreenEvent((xTouchEvent*) &currentTouch);
    }else if (!alreadyPoped){
      currentTouch.event = popTs;
      alreadyPoped = true;
      Serial.printf(F_WRP("pop x: %d; y: %d; m: %d;\n"), currentTouch.x, currentTouch.y, currentTouch.eventTime);
      bWindowManagerCheckTouchScreenEvent((xTouchEvent*) &currentTouch);
    }

    
    bTouchInt = false;
    lastTouched = touchFlag;
  

  return touchFlag;
}
