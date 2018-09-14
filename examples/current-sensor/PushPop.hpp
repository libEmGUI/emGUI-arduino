#pragma once

#include "Timeout.hpp"



class PushPop {
public:

	typedef enum {
		PUSHED,
		POPED,
		LONGPRESS,
		NONE,
	} RETURN_VALS_t;

	typedef enum {
		PUSH,
		POP,
	} PUSH_STATES_t;
	PushPop() {
		_state = POP;
		_clearedLongpress = true;
	}

	RETURN_VALS_t push() {
		if (_state == POP) {
			Serial.println("Button pushed");
			_clearedLongpress = false;
			timeout.reset();
			_state = PUSH;
			return PUSHED;
		}
		else return NONE;
	}

	RETURN_VALS_t isLongPressed() {
		if (_state != PUSH) return NONE;
		
		if (timeout.isOver()&& !_clearedLongpress) 
		{	
			_clearedLongpress = true;
			return LONGPRESS;
		} else {
			return NONE;
		}
	}

	

	RETURN_VALS_t pop() {
		if (_state == PUSH) {
			_state = POP;
			if (timeout.isOver() && !_clearedLongpress) return LONGPRESS;
			else return POPED;
		}
		else return NONE;
	}

protected:
	Timeout timeout = Timeout(3000);
	PUSH_STATES_t _state;
	bool _clearedLongpress;
};
