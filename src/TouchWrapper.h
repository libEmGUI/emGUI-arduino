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

		template<typename T, size_t N> 
		TouchPoint transform(T (&matrix)[N]){
			size_t matrixSize = 3;
			if (N != matrixSize*matrixSize)
				return TouchPoint();

			T point[] = {(T)x, (T)y, (T)1};
			T transformed[] = {(T)0, (T)0, (T)0};

			for(size_t i = 0; i < matrixSize; i++){
				for(size_t j = 0; j < matrixSize; j++){
					transformed[i] += point[j] * matrix [i*matrixSize + j]; 
				}
			}

			return TouchPoint(transformed[0], transformed[1]);
		}
	};

	TouchWrapper(Adafruit_GFX &tft);
	bool loop();

	virtual bool isTouched() = 0;
	virtual TouchPoint getTouchCoords() = 0;

	// Applies _transformMatrix DOT matrix
	template<size_t N>
	void dotTransformation(float (&matrix)[N]) {
		size_t matrixSize = 3;
		if (N != matrixSize*matrixSize)
			return;

		float _tmpMatrix[matrixSize][matrixSize];

		memcpy(_tmpMatrix, _transformMatrix, sizeof(_tmpMatrix));
		
		for(size_t i = 0; i < matrixSize; i++){
			for(size_t j = 0; j < matrixSize; j++){
				float tmp = 0.f;
				for(size_t k = 0; k < matrixSize; k++){
					tmp += _transformMatrix[k + matrixSize*i] * matrix[k*matrixSize + j];
				}
				_tmpMatrix[i][j] = tmp;
			}
		}

		//apply
		memcpy(_transformMatrix, _tmpMatrix, sizeof(_transformMatrix));
	}

	void dumpMatrix(){
		size_t matrixSize = 3;
		Serial.printf("Transformation Matrix is: \n");
		for(size_t i = 0; i < matrixSize; i++){
			for(size_t j = 0; j < matrixSize; j++){
				Serial.printf("%f, ", _transformMatrix[i*matrixSize + j]);
			}
			Serial.printf("\n");
		}
	}

protected:
	volatile xTouchEvent currentTouch;
	volatile bool bTouchInt = false;
	bool alreadyPoped = true;
	bool lastTouched = false;

	// I matrix first
	float _transformMatrix[9] = {
		1.0f, 0.f, 0.f,
		0.f, 1.0f, 0.f,
		0.f, 0.f, 1.f
	};
	Adafruit_GFX &_tft;
};
