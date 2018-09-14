#ifndef _UPDATABLE_VALUE_
#define _UPDATABLE_VALUE_

class UpdatableValue {
public:
	UpdatableValue(): 
		_value(0), 
		_updated(true) {}

	int operator ++(int val) {
		_value++;
		_updated = true;
		return val;
	}

	void operator()() {
		(*this)++;
	}

	int operator =(int value) {
		if (value == _value) return _value;

		_value = value;
		_updated = true;
		return _value;
	}

	operator bool() const {
		return _updated;
	}

	int pick(){
		return _value;
	}

	operator int() const {
		return _value;
	}

	int operator*(int lhs) {
		return _value * lhs;
	}

	int operator+(int lhs) {
		return _value + lhs;
	}

	int operator/(int lhs) {
		return _value / lhs;
	}

	int operator-(int lhs) {
		return _value - lhs;
	}

	int operator==(int lhs) {
		return _value == lhs;
	}

	int v() {
		_updated = false;
		return _value;
	}
protected:
	int _value;
	bool _updated;
};

#endif //