#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "../common/Timeout.hpp"
#include "../common/Config.hpp"

class deviceState {
public:
	deviceState();

	bool parseUpdate(JsonObject& upd);

	bool parseConfig(JsonObject&);

	static deviceState* getInstance();

	void setConfig();
	void loop();

	typedef enum {
		FIRMWARE,
		SPIFFS,
	} updFlag_t;

	bool _debug = false;
	

	String _newSsid;
	String _newPass;

	bool _newConfigSaved = true;
	
	bool serviceMode = false;
	bool mqttReady();

	typedef enum {
		WIFI_DISC,
		WIFI_CONN,
		WIFI_GOTIP
	} wifi_t;

	wifi_t wifiStatus;

	String ssid = "";

protected:
	Timeout _respTimeout = Timeout(20000);
	Timeout _mqttConTimeout = Timeout(20000);
	String _updURL = "";
	updFlag_t _updFlag = FIRMWARE;

	typedef enum {
		DISC,
		SUB_Auth,
		SUB_SetWifi,
		SUB_Refresh,
		SUB_Upd,
		SUB_Job,
		ASK_Job,
		RDY
	}connState_t;

	connState_t connState = DISC;

	String payloadBuf;

	String shopID = "100";
	StaticJsonBuffer<1800> jsonBuffer;

	bool _auth = false;
};

