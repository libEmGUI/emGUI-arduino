#include "CommonWifi.h"

#include <ArduinoJson.h>
#include "Config.hpp"
#include "../GUI/deviceState.hpp"

String initWifi() {
	JSONConfig jc("/config.json");
	auto &j = jc.load();
	auto parsed_ssid = j[F("wifi")][F("ssid")].as<char*>();
	auto password = j[F("wifi")][F("pass")].as<char*>();

    if (deviceState::getInstance()->serviceMode) {
        parsed_ssid = "smartwatch";
        password = "12345678";
    }

	if (!(password && parsed_ssid)) return "";

	String ssid = parsed_ssid;
	if (!ssid.length()) {
		Serial.print(F("Invalid wifi config"));
		return ""; //TODO: fire event
	}
  

	Serial.print(F("Connecting to WiFi "));
	Serial.println(ssid);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid.c_str(), password);
	WiFi.setOutputPower(0.f);
    
    return ssid;
	// TODO: ENERGY CONSUMPTION !!!
  //wifi_set_sleep_type(LIGHT_SLEEP_T);
}
