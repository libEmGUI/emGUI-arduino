/********************************************************************
*
*   Config class is a wrapper for configuration file
*
*********************************************************************/
#ifndef _CONFIG_FILE_
#define _CONFIG_FILE_

#include <Arduino.h>
#include <ArduinoJson.h>

class JSONConfig {
public:
	JSONConfig(const char *fn);
#if !defined(_MSC_VER) && defined(ARDUINOJSON_ENABLE_ARDUINO_STRING)
	JSONConfig(const __FlashStringHelper *fn);
#else
#endif // !1

	bool exists();
	bool remove();
	JsonObject &load();
	JsonObject &create() {
		return jsonBuffer.createObject();
	};
	bool save(JsonObject &root);
protected:
	String _fn;
	DynamicJsonBuffer jsonBuffer;
};

template <size_t CAPACITY>
class BufferedPrint : public Print {
public:
	BufferedPrint() : _size(0) {
		*_buffer = 0;
	}

	virtual size_t write(uint8_t c) {
		if (_size + 2 == CAPACITY) {
			return 0;
		}

		_buffer[_size++] = c;
		_buffer[_size] = 0; //terminator
		return 1;
	}

	const char * c_str() {
		return _buffer;
	};

private:
	size_t _size;
	char _buffer[CAPACITY];
};

#endif //_CONFIG_FILE_