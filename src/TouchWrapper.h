#pragma once

#include <Adafruit_GFX.h>
#include <emGUI.h>

class TouchWrapper {
public:
	typedef struct TouchPoint TouchPoint_t;
	struct TouchPoint {
		TouchPoint():
			x(0),
			y(0){};
		TouchPoint(int newX, int newY): x(newX), y(newY){}; 
		int x;
		int y;
		TouchPoint transform(float * matrix, size_t size);
	};

	TouchWrapper(Adafruit_GFX &tft);
	bool loop();

	virtual bool isTouched() = 0;
	virtual TouchPoint getTouchCoords() = 0;

protected:
	volatile xTouchEvent currentTouch;
	volatile bool bTouchInt = false;
	bool alreadyPoped = true;
	bool lastTouched = false;
	float xScale = 1.0f;
	float yScale = 1.0f;
	int xOffset = 0;
	int yOffset = 0;
	int xMax = 100;
	int yMax = 100;
	Adafruit_GFX &_tft;
};
