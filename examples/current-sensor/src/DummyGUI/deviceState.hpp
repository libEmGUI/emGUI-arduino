#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "../../Timeout.hpp"
#include <AsyncMqttClient.hpp>
#include "../../Config.hpp"

class deviceState {
public:
	deviceState();

	typedef enum {
		DeviceStateUNAUTH,
	}deviceState_t;

	void mqttServiceMode();
	bool debug();
	void debug(bool state);

	bool parseTask(JsonObject& task);

	bool parseUpdate(JsonObject& upd);

	bool parseConfig(JsonObject&);

	String& message() {
		return _taskMessage;
	}

	bool hasTask() {
		return _taskId > 0;
	}

	bool topicEndsWith(const char * topic, const char * substr) {
		auto substrTopic = strstr(topic, substr);
		//substr found in topic, check if it is in the end
		if (substrTopic && strcmp(substrTopic, substr) == 0)
			return true;

		return false;
	}

	AsyncMqttClient& mqtt() {
		return _mqtt;
	};

	void showTask();

	void responseTask(int option);

	void setServiceMode(bool);
	static deviceState* getInstance();

	void setConfig();
	void loop();

	typedef enum {
		FIRMWARE,
		SPIFFS,
	} updFlag_t;

	bool _debug = false;
	
	bool _isInSeviceMode = false;
	String mqttServiceModeHost = "";
	String _taskMessage;
	String _taskTitle;
	String _taskType;
	int    _taskId = 0;
	bool   _taskShowed = true;
	bool   _taskResponded = true;
	String _taskOption1;
	String _taskOption2;

	String _newSsid;
	String _newPass;
	String _newMqttAddr = "test.flexibity.com";
	int    _newMqttPort = 1883;
	String _newMqttUser = "smartwatch";
	String _newMqttPass = "trytohackme";

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

	AsyncMqttClient _mqtt;

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

