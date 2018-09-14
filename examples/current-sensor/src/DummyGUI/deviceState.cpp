#include "deviceState.hpp"
#include "WindowPack.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

deviceState::deviceState() {
	JSONConfig jc("/config.json");
	auto &j = jc.load();
	auto addr = j[F("mqtt")][F("addr")].as<char *>();
	auto port = j[F("mqtt")][F("port")].as<int>();
	if (addr && port) _mqtt.setServer(addr, port);
	auto user = j[F("mqtt")][F("user")].as<char *>();
	auto pass = j[F("mqtt")][F("pass")].as<char *>();
	if (user && pass) _mqtt.setCredentials(user, pass);

	_mqtt.setKeepAlive(5);
	auto t2 = String(F("shop/")) + shopID + String(F("/")) + WiFi.macAddress() + String(F("/lastwill"));
	_mqtt.setWill(t2.c_str(), 0, false, "hi" ,2);
	Serial.print(F("\nLWT: "));
	Serial.println(t2);

	_mqtt
		.onConnect([this](bool sessionPresent) {
		Serial.println(F("\nMQTT Connected!"));
		_mqttConTimeout.reset();
		connState = DISC;
		
		auto t = String(F("shop/")) + shopID + String(F("/message/wristlet/")) + WiFi.macAddress() + String(F("/info"));
		Serial.print(F("\nSubscribing to "));
		Serial.println(t);
		if (_mqtt.subscribe(t.c_str(), 2))
			connState = SUB_Auth;
		else {
			Serial.print(F("\nDisconnect on "));
			Serial.println(connState);
		}


		
	}).onSubscribe([this](uint16_t packetId, uint8_t qos) {
		switch (connState) {
		case SUB_Auth:
		{
			auto t = String(F("shop/")) + shopID + String(F("/message/wristlet/")) + WiFi.macAddress() + String(F("/job"));
			Serial.print(F("\nSubscribing to "));
			Serial.println(t);
			if (_mqtt.subscribe(t.c_str(), 2))
				connState = SUB_SetWifi;
			else {
				Serial.print(F("\nDisconnect on "));
				Serial.println(connState);
			}
		}
		break;
		case SUB_SetWifi:
		{
			auto t = String("smartwatch/set/config/" + WiFi.macAddress());
			Serial.print(F("\nSubscribing to "));
			Serial.println(t);
			if (_mqtt.subscribe(t.c_str(), 2))
				connState = SUB_Refresh;
			else {
				Serial.print(F("\nDisconnect on "));
				Serial.println(connState);
			}
		}
		break;
		case SUB_Refresh:{
			auto t = String(F("shop/")) + shopID + String(F("/refresh"));
			Serial.print(F("\nSubscribing to "));
			Serial.println(t);
			if (_mqtt.subscribe(t.c_str(), 2))
				connState = SUB_Upd;
			else {
				Serial.print(F("\nDisconnect on "));
				Serial.println(connState);
			}
		}
		break;
		case SUB_Upd:
		{
			auto t = String(F("smartwatch/update/+"));
			
			Serial.print(F("\nSubscribing to "));
			Serial.println(t);
			if (_mqtt.subscribe(t.c_str(), 2))
				connState = SUB_Job;
			else {
				Serial.print(F("\nDisconnect on "));
				Serial.println(connState);
			}
		}
		break;
		case SUB_Job: {
			connState = ASK_Job;
			Serial.println(F("\nSubscribed all topics!"));
			auto t = String(F("driven/")) + shopID + String(F("/message/wristlet/")) + WiFi.macAddress() + String(F("/new"));
			if (_mqtt.publish(t.c_str(), 1, 0)) {
				Serial.print(F("\npub to "));
				Serial.println(t);
			}
			else {
				Serial.print(F("\nDisconnect on "));
				Serial.println(connState);
			}
			break;
		}
		default:
			Serial.println(F("\nUnknown State"));
			_mqtt.disconnect();
			break;
		}
	}).onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
		Serial.println(F("\nMQTT Disconnected!"));
		connState = DISC;
		if (WiFi.isConnected())
			_mqtt.connect(); //restore connection
	}).onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
		if (index == 0) {
			payloadBuf = "";
			jsonBuffer.clear();
		}

		auto pl = len;
		auto p = payload;
		while (pl--) {
			payloadBuf += *(p++);
		}


		if (index + len == total) {
			JsonObject& root = jsonBuffer.parseObject(payloadBuf);
			if (root.success()) {
				Serial.print(F("\nSucessfully parsed JSON: "));
				root.prettyPrintTo(Serial);

				if (topicEndsWith(topic, F_WRP("/job"))) {
					Serial.println(F("\nNew Job arrived!"));
					deviceState::getInstance()->parseTask(root);
				}

				if (topicEndsWith(topic, F_WRP("/refresh"))) {
					Serial.println(F("\nRefresh!"));
					auto t = String(F("driven/")) + shopID + String(F("/message/wristlet/")) + WiFi.macAddress() + String(F("/new"));
					if (_mqtt.publish(t.c_str(), 1, 0)) {
						Serial.print(F("\npub to "));
						Serial.println(t);
					}
					else {
						Serial.print(F("\nDisconnect on "));
						Serial.println(connState);
					}
					
				}

				if (!strcmp(topic, F_WRP("smartwatch/update/fw"))) {
					Serial.println(F("\nNew firmware update arrived!"));
					deviceState::getInstance()->parseUpdate(root);
					_updFlag = deviceState::FIRMWARE;
				}

				if (!strcmp(topic, F_WRP("smartwatch/update/fs"))) {
					Serial.println(F("\nNew Spiffs update arrived!"));
					deviceState::getInstance()->parseUpdate(root);
					_updFlag = deviceState::SPIFFS;
				}
				auto t = String("smartwatch/set/config/" + WiFi.macAddress());
				if (!strcmp(topic, t.c_str())) {
					Serial.println(F("\nNew wifi credentials arrived!"));
					deviceState::getInstance()->parseConfig(root);
					_updFlag = deviceState::SPIFFS;
				}


			}
			else {
				Serial.println(F("\nFailed parse JSON from payload: "));
				Serial.println(payloadBuf);
			}
		}

		return;
	}).onPublish([this](uint16_t packetId) {
		switch (connState) {
		case ASK_Job: {
			connState = RDY;
			//auto t = String(F("driven/")) + shopID + String(F("/job/")) + WiFi.macAddress() + String(F("/request"));
			auto t = String(F("driven/job/request"));
			if (_mqtt.publish(t.c_str(), 2, 0)) {
				Serial.print(F("\nAsk new job to "));
				Serial.println(t);
			}
			else {
				Serial.print(F("\nDisconnect on "));
				Serial.println(connState);
				_mqtt.disconnect();
			}
			break;
		}
		}
	});
}


void deviceState::mqttServiceMode(){
	_mqtt.disconnect();
	_isInSeviceMode = true;
	auto addr = mqttServiceModeHost;
	auto port = 1883;
	auto user = "";
	auto pass = "";
	Serial.println(" === Service MQTT: === ");
	Serial.printf( " host: %s \r\n", addr.c_str());
	Serial.printf( " port: %d \r\n", port);
	Serial.printf( " user: %s \r\n", user);
	Serial.printf( " pass: %s \r\n", pass);
	if (addr && port) _mqtt.setServer(addr.c_str(), port);
	if (user && pass) _mqtt.setCredentials(user, pass);
}

bool deviceState::mqttReady(){
	return (connState == RDY);
}

bool deviceState::parseUpdate(JsonObject& upd) {
	_updURL = upd[F("body")][F("URL")].as<char *>();
	return true;
}


bool deviceState::debug() {
	return _debug;
}

void deviceState::debug(bool state) {
	_debug = state;
}

bool deviceState::parseTask(JsonObject& task) {

	auto empty = "";
	
	if (auto v = task[F("body")][F("title")].as<char *>())
		_taskTitle = v;
	else
		_taskTitle = empty;

	if (auto v = task[F("body")][F("message")].as<char *>())
		_taskMessage = v;
	else
		_taskMessage = empty;

	if (auto v = task[F("body")][F("type")].as<char *>())
		_taskType = v;
	else
		_taskType = empty;

	
	if (auto v = task[F("body")][F("options")][0].as<char *>())
		_taskOption1 = v;
	else
		_taskOption1 = empty;
	
	if (auto v = task[F("body")][F("options")][1].as<char *>())
		_taskOption2 = v;
	else
		_taskOption2 = empty;
	 
	_taskId = task[F("body")][F("id")].as<int>();

	if( _taskId && _taskMessage.length())
		_taskShowed = false;

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

	if (auto v = cred[F("body")][F("mqtt")][F("addr")].as<char *>()){
		if (((String)v).length() == 0) {
			_newConfigSaved = true;
		} else {
			_newMqttAddr = v;
		}
	}
	else
		_newConfigSaved = true;

	if (auto v = cred[F("body")][F("mqtt")][F("port")].as<int>()){
		if (((String)v).length() == 0) {
			_newConfigSaved = true;
		} else {
			_newMqttPort = v;
		}
	}
	else
		_newConfigSaved = true;

	if (auto v = cred[F("body")][F("mqtt")][F("user")].as<char *>()){
		if (((String)v).length() == 0) {
			_newConfigSaved = true;
		} else {
			_newMqttUser = v;
		}
	}
	else
		_newConfigSaved = true;

	if (auto v = cred[F("body")][F("mqtt")][F("pass")].as<char *>()){
		if (((String)v).length() == 0) {
			_newConfigSaved = true;
		} else {
			_newMqttPass = v;
		}
	}
	else
		_newConfigSaved = true;
	return true;
}

void deviceState::showTask() {
	if (_taskId) {
		Peripherial::getInstance()->wakeUp();

		if(WindowTask::getInstance(false))  //to update labels
			WindowTask::getInstance()->close();

		WindowTask::getInstance()->open();
		Peripherial::getInstance()->vibroPulse();
		Serial.print(F("\nPopping up window for task "));
		Serial.println(_taskId);
	}

	_taskShowed = true;
	_taskResponded = false;
	_respTimeout.reset();
}

void deviceState::setConfig(){
	Serial.println("Saving new config...");
	JSONConfig jc("/config.json");
	auto &j = jc.load();
	j[F("wifi")][F("ssid")] = _newSsid;
	j[F("wifi")][F("pass")] = _newPass;

	j[F("mqtt")][F("addr")] = _newMqttAddr;
	j[F("mqtt")][F("port")] = _newMqttPort;
	j[F("mqtt")][F("user")] = _newMqttUser;
	j[F("mqtt")][F("pass")] = _newMqttPass;
	jc.save(j);

	auto dial = iModalDialogOpen(0, "y", "New config!", "New config has came");
	vModalDialogSetHandler(dial, NULL, [](char cBtn, void* ctx) -> bool {
		(void)ctx;
      if (cBtn == 'y'){
        Peripherial::getInstance()->reset();
      }

      return true; // dialog can close on this button
    });
	_newConfigSaved = true;
}
void deviceState::loop() {
	if (!_taskShowed)
		showTask();

	if (!_newConfigSaved){
		setConfig();
	}
	if (_taskId && !_taskResponded && _respTimeout.isOver()) {
		_respTimeout.reset();
		Peripherial::getInstance()->vibroPulse();
	}

	if (_mqtt.connected()) {
		_mqttConTimeout.reset();
	}

	if (_mqttConTimeout.isOver()) {
		if (WiFi.isConnected()) {
			_mqtt.connect();
			_mqttConTimeout.reset();
			Serial.println(F("\nReconnecting MQTT by timeout"));
		}
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

void deviceState::responseTask(int option) {
	if (_taskResponded)
		return;

	//auto t = String(F("driven/")) + shopID + String(F("/job/")) + WiFi.macAddress() + String(F("/request"));
	auto t = String(F("driven/job/")) + WiFi.macAddress() + String(F("/result/")) ;

	DynamicJsonBuffer jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();
	{
		auto &props = root.createNestedObject(F("properties"));

		props[F("drivenMessageName")] = F("DrivenJobStatusMessage");
		props[F("drivenMessageVersion")] = F("1.0");
		props[F("drivenTarget")] = F("1.0");
		props[F("drivenSource")] = String(F("watch_")) + WiFi.macAddress();
	}

	{
		auto &body = root.createNestedObject(F("body"));
		body[F("id")] = _taskId;
		body[F("option")] = option;
	}
	t += option;
	auto jb = std::make_shared<BufferedPrint<312>>();
	root.printTo(*jb);

	if (_mqtt.publish(t.c_str(), 2, 0, jb->c_str())) {
		Serial.print(F("\nRespond to "));
		Serial.println(t);
		Serial.println(F("\nWith MSG "));
		Serial.println(jb->c_str());
		_taskResponded = true;
		_taskResponded = true;
	}
	else {
		Serial.print(F("\nDisconnect on respond"));
		_mqtt.disconnect();
	}
}