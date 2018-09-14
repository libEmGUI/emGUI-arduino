#include "Config.hpp"
#include "FS.h"

JSONConfig::JSONConfig(const char *fn) {
	_fn = fn;
	SPIFFS.begin();
};

#if !defined(_MSC_VER) && defined(ARDUINOJSON_ENABLE_ARDUINO_STRING)
JSONConfig::JSONConfig(const __FlashStringHelper *fn) {
	_fn = fn;
	SPIFFS.begin();
}
#else 

#endif

JsonObject &JSONConfig::load() {

	File file = SPIFFS.open(_fn, "r");

	// Parse the root object
	JsonObject &root = jsonBuffer.parseObject(file);

	if (!root.success()) {
		Serial.print(F("Failed to read file "));
		Serial.println(_fn);
	}

	root.prettyPrintTo(Serial);
	Serial.println();

	file.close();

	return root;
}

bool JSONConfig::save(JsonObject &root) {
	
	SPIFFS.remove(_fn);
	File file = SPIFFS.open(_fn, "w");
	if (!file) {
		Serial.println(F("Failed to create file"));
		return false;
	}

	root.prettyPrintTo(file);

	file.close();
	return true;
}

bool JSONConfig::remove() {
	return SPIFFS.remove(_fn);
}

bool JSONConfig::exists() {
	return SPIFFS.exists(_fn);
}
