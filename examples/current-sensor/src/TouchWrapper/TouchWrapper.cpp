#include "TouchWrapper.h"

void TouchWrapper::init(bool debug)
{
	Serial.println("configuring touch");
	begin();

	bTouchInt = false;
	alreadyPoped = true;

	lastTouched = false;
}

bool TouchWrapper::handleTouch(bool flag)
{
	currentTouch.eventTime = millis();
	bool touchFlag = touched();

	if (touchFlag)
	{
		alreadyPoped = false;
		uint16_t xptr;
		uint16_t yptr;

		TS_Point p = getPoint();

		currentTouch.x = 320 - ((p.x - 300) * 10 / 115); // * xCompensationA / mul + xCompensationB;
		currentTouch.y = (p.y - 300) * 10 / 154;		 // * yCompensationA / mul + yCompensationB;

		if (!lastTouched)
		{
			currentTouch.event = pushTs;
			//Serial.printf(F_WRP("push x: %d; y: %d; m: %d;\n"), currentTouch.x, currentTouch.y, currentTouch.eventTime);
		}
		else
		{
			currentTouch.event = updateTs;
			//Serial.printf(F_WRP("upd x: %d; y: %d; m: %d;\n"), currentTouch.x, currentTouch.y, currentTouch.eventTime);
		}
		bWindowManagerCheckTouchScreenEvent((xTouchEvent *)&currentTouch);
	}
	else if (!alreadyPoped)
	{
		currentTouch.event = popTs;
		alreadyPoped = true;
		Serial.printf(F_WRP("pop x: %d; y: %d; m: %d;\n"), currentTouch.x, currentTouch.y, currentTouch.eventTime);
		bWindowManagerCheckTouchScreenEvent((xTouchEvent *)&currentTouch);
	}

	bTouchInt = false;
	lastTouched = touchFlag;

	return touchFlag;
}
