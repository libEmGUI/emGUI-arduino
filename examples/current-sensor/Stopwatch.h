/*********************************************************************
*
*	Easy to use header only stopwatch for Arduino. (in millis)
*
*	Start it with 
*		`start()`
*
*	To pause use 
*		`stop()` (returns intermediate result)
*
*	And finaly reset it with 
*		`start()`  (returns result)
*
*	`start()` will begin loop from start
*  
*********************************************************************/
#ifndef _BAT_TIMER_
#define _BAT_TIMER_

#include <Arduino.h>


class Stopwatch{
	public:
		Stopwatch(){}
		unsigned long start(){
			if (_stoped) {
				_startTime = millis();
				_stoped = false;
			}

			return _startTime;
		}

		unsigned long stop(){
			auto now = getNow();

			if (!_stoped){
				_stopTime = now;
				_resultTime += _stopTime - _startTime;
			}
			_stoped = true;

			return _resultTime;
		}


		unsigned long reset(){
			_startTime= 0;
			_stopTime = 0;
			_resultTime = 0;

			return _resultTime;
		}

		unsigned long get(){
			auto now = getNow();

			if (_stoped){
				return _resultTime;
			} else {
				return _resultTime + (now - _startTime);
			}
		}

		unsigned long getNow(){
			auto now = millis();
			if (now < _startTime) {
				_resultTime = 0xFFFFFFFF - _startTime + now;
				_startTime = now; 
			}

			return now;
		}
		char a[100];

		char * getFormatedResult(){
			int sec = get() / 1000;
			int h = sec / 3600;
			int m = sec / 60 - h * 60;
			int s = sec - m * 60 - h * 3600;
			snprintf(a, 100, "%2d:%02d:%02d", h, m, s);
			return a;
		}		
	private:
		bool _stoped = true;
		unsigned long _stopTime = 0;
		unsigned long _startTime = 0;
		unsigned long _endTime = 0;
		unsigned long _resultTime = 0;

};



#endif //_BAT_TIMER_