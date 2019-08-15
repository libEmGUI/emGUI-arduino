#include "TouchWrapper.h"


TouchWrapper::TouchWrapper(Adafruit_GFX &tft): _tft(tft){
	bTouchInt = false;
	alreadyPoped = true;
	lastTouched = false;
}

TouchWrapper::TouchPoint TouchWrapper::TouchPoint::transform(float * matrix, size_t size){
	TouchPoint tp;

	size_t matrixSize = 3;
	if (size != matrixSize*matrixSize)
		return tp;

	float point[] = {(float)x, (float)y, 1.f};
	float transformed[] = {0.0f, 0.0f, 0.0f};

	for(size_t i = 0; i < matrixSize; i++){
		for(size_t j = 0; j < matrixSize; j++){
			transformed[i] += point[j] * matrix [i*matrixSize + j]; 
		}
	}

	tp.x = (int) transformed[0];
	tp.y = (int) transformed[1];
	return tp;
}

bool TouchWrapper::loop()
{
	currentTouch.eventTime = millis();
	bool touchFlag = isTouched();

	if (touchFlag)
	{
		alreadyPoped = false;
		
		auto p = getTouchCoords();

		if (p.x < 0) p.x = 0;
		if (p.x > _tft.width()) p.x = _tft.width();

		if (p.y < 0) p.y = 0;
		if (p.y > _tft.height()) p.y = _tft.height();

		currentTouch.x = p.x;
		currentTouch.y = p.y;

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
