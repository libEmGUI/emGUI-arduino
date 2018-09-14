#include <Arduino.h>
#include "IIR_filter.h"

IIR_filter::IIR_filter(int timeBase) {
	setCoef(timeBase);
	lastValue = 0;
	_prev =millis();
}

int IIR_filter::doSample(int data) {
	static int firstCall = true;
	auto now = millis();
	int c = (now - _prev);
	_prev = now;

	int answer;

	if (firstCall){
		answer = data;
		firstCall = false;
	} else {
		if (c > timeBase/2 ) {
			answer = data;
		} else {
			int delta = data - lastValue;
			delta = delta * c / timeBase;
			answer = lastValue + delta;
		}
	}
	
	lastValue = answer;
	return lastValue;
}

bool IIR_filter::setCoef(int c) { 
	if (c >= 0) {
		timeBase = c;
		return true;
	} else {
		timeBase = 0;
		return false;
	}

}

int IIR_filter::getCoef() {
	return timeBase;
}
IIR_filter::~IIR_filter()
{
}
