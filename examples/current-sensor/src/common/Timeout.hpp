#ifndef TIMEOUT_H_INCLUDED
#define TIMEOUT_H_INCLUDED

#include "Arduino.h"

class Timeout {
public:
	Timeout(unsigned long int duration): _duration(duration){
		start();
	};

	bool isOver() {
		if (_stoped) return false;
		return (_endTime <= millis());
	}


	void reset() {
		_startTime
 = millis();
		_endTime = _startTime
 + _duration;
	}

	void stop(){
		_stoped = true;
	}

	void start(){
		_startTime = millis();
		_endTime = _startTime + _duration;
		_stoped = false;
	} 
private:
	unsigned long int _duration;
	unsigned long int _startTime;
	unsigned long int _endTime;
	bool _stoped = false;
};


#endif // TIMEOUT_H_INCLUDED
