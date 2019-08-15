#include "TouchWrapper.h"


TouchWrapper::TouchWrapper(Adafruit_GFX &tft): _tft(tft){
	bTouchInt = false;
	alreadyPoped = true;
	lastTouched = false;
}

bool TouchWrapper::loop()
{
	currentTouch.eventTime = millis();
	bool touchFlag = isTouched();

	if (touchFlag)
	{
		alreadyPoped = false;
		
		auto p = getTouchCoords();

		auto pt = TouchPoint(p.x, p.y).transform(_transformMatrix);

		if (pt.x < 0) pt.x = 0;
		if (pt.x > _tft.width()) pt.x = _tft.width();

		if (pt.y < 0) p.y = 0;
		if (pt.y > _tft.height()) pt.y = _tft.height();

		currentTouch.x = pt.x;
		currentTouch.y = pt.y;

		if (!lastTouched)
			currentTouch.event = pushTs;
		else
			currentTouch.event = updateTs;

		bWindowManagerCheckTouchScreenEvent((xTouchEvent *)&currentTouch);
	}
	else if (!alreadyPoped)
	{
		currentTouch.event = popTs;
		alreadyPoped = true;
		//Serial.printf(F_WRP("pop x: %d; y: %d; m: %d;\n"), currentTouch.x, currentTouch.y, currentTouch.eventTime);
		bWindowManagerCheckTouchScreenEvent((xTouchEvent *)&currentTouch);
	}

	bTouchInt = false;
	lastTouched = touchFlag;

	return touchFlag;
}
