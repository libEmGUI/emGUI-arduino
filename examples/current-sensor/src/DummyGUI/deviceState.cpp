#include "deviceState.hpp"
#include "WindowPack.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

deviceState::deviceState() {
	JSONConfig jc("/config.json");
	auto &j = jc.load();
}

bool deviceState::parseUpdate(JsonObject& upd) {
	_updURL = upd[F("body")][F("URL")].as<char *>();
	return true;
}


bool deviceState::parseConfig(JsonObject& cred) {
	//TODO: this is dummy logic! Refactor 
	auto empty = "";
	_newConfigSaved = false;
	if (auto v = cred[F("body")][F("wifi")][F("ssid")].as<char *>()){
		if (((String)v).length() == 0) {
			_newConfigSaved = true;
		} else {
			_newSsid = v;
		}
	}
	else {
		_newConfigSaved = true;
	}

	if (auto v = cred[F("body")][F("wifi")][F("pass")].as<char *>()){
		if (((String)v).length() == 0) {
			_newConfigSaved = true;
		} else {
			_newPass = v;
		}
	}
	else
		_newConfigSaved = true;

	return true;
}


void deviceState::setConfig(){
	Serial.println("Saving new config...");
	JSONConfig jc("/config.json");
	auto &j = jc.load();
	j[F("wifi")][F("ssid")] = _newSsid;
	j[F("wifi")][F("pass")] = _newPass;

	jc.save(j);

	auto dial = iModalDialogOpen(0, "y", "New config!", "New config has came");
	vModalDialogSetHandler(dial, NULL, [](char cBtn, void* ctx) -> bool {
		(void)ctx;
      if (cBtn == 'y'){
      }

      return true; // dialog can close on this button
    });
	_newConfigSaved = true;
}


void deviceState::loop() {
	if (!_newConfigSaved){
		setConfig();
	}

	if (_updURL.length()> 1) {
		if (_updFlag == FIRMWARE) {
			Serial.println(F("\Starting firmware update..."));
			t_httpUpdate_return ret = ESPhttpUpdate.update(_updURL, "");
			switch (ret) {
			case HTTP_UPDATE_FAILED:
				Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
				break;

			case HTTP_UPDATE_NO_UPDATES:
				Serial.println("HTTP_UPDATE_NO_UPDATES");
				break;

			case HTTP_UPDATE_OK:
				Serial.println("HTTP_UPDATE_OK");
				break;
			}
			_updURL = "";
		}
		else if (_updFlag == SPIFFS) {
			Serial.println("\Starting spiffs update...");
			t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(_updURL, "");
			switch (ret) {
			case HTTP_UPDATE_FAILED:
				Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
				break;

			case HTTP_UPDATE_NO_UPDATES:
				Serial.println("HTTP_UPDATE_NO_UPDATES");
				break;

			case HTTP_UPDATE_OK:
				Serial.println("HTTP_UPDATE_OK");
				break;
			}
			_updURL = "";
		}
	}
	
}


deviceState* deviceState::getInstance() {
	static deviceState *instance = nullptr;

	if (instance) return instance;

	instance = new deviceState();

	return instance;
};
