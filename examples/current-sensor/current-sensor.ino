/*
*  SW project main file. 
*  
*  All about port extender is in Extender and Peripherial
*  TFT configs are in User_Setup.h. wrapper for tft - TFTWtapper
*  Flexibity 2017 // Roman Savrulin, Mikhail Natalenko
*/

#include <Wire.h>
#include <Hash.h>
#include <FS.h>
#include <AsyncMqttClient.hpp>

#include "GUI.h"
#include <emGUI.h>
#include "Peripherial.h"
#include "emGUIGlue.h"
#include "TouchWrapper.h"
#include "I2CBitbanger.h"

#include <sys/time.h>
#include "Timeout.hpp"

#include <time.h>  
#include <coredecls.h>                  // settimeofday_cb()
#include "src/DummyGUI/deviceState.hpp"

#include "src/DummyGUI/WindowPack.h"

#include "CommonWifi.h" 
#include "Version.h"


#define TZ              3       // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

	
timeval cbtime;			// time set in callback
bool cbtime_set = false;

void time_is_set(void)
{
	gettimeofday(&cbtime, NULL);
	cbtime_set = true;
	Serial.println(F("Time was updated"));
}

extern "C" {
#include "user_interface.h"
}

#include "User_Setup.h"


/************ Global vars and constants ******************************/
static Peripherial* periph = Peripherial::getInstance();                    // port extender (buttons, vibro, charger status)

WiFiEventHandler onConnect, onDisconnect, onIp;

/****************** Interrupt handlers   **********************/
void ICACHE_RAM_ATTR handleSysInt() {
  periph->intr();
}

void ICACHE_RAM_ATTR handleTouchInt() {
  periph->touch.init();
}

void ICACHE_RAM_ATTR handleNfcInt() {
  periph->intrNFC();
}


/***********************  Peripherial Events        *************************/
static void PeriphEventHdl(Peripherial *sender, Peripherial::eventType event, int32_t param1) {
  vGUIHandlePeriph((eHW)event, param1);
  switch (event) {
    case Peripherial::EV_BTN_PRESS:
      bWindowManagerCheckKeypressEvent(param1);
      break;

  }
}

void setup() {
	settimeofday_cb(time_is_set);
#ifndef _MSC_VER
	WiFi.disconnect();
	configTime(TZ_SEC, DST_SEC, "pool.ntp.org"); //TODO time servers!
#endif // MSVC_VER

	// TODO: ENERGY CONSUMPTION !!!
  //wifi_set_sleep_type(LIGHT_SLEEP_T);

  if(digitalRead(UART_RX) == 1) {
    if(!Serial){
      Serial.begin(115200);
      while (!Serial){
        delay(100);
      }
    }
  }

  Serial.println(F("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\r\n"));
  delay(100);
  Serial.println(F("Ready"));

  Serial.print(F("MAC: "));
  Serial.println(WiFi.macAddress());

  Serial.print(F("Core: "));
  //Serial.println(ESP.getCoreVersion());

  Serial.print(F("SDK: "));
  //Serial.println(ESP.getSdkVersion());

  SPIFFS.begin();


  pinMode(INTR_SYS, INPUT_PULLUP);
  //pinMode(INTR_TOUCH, INPUT_PULLUP);

  I2CBitbanger(SDA_PIN, SCK_PIN);
  
  periph->init();
  periph->onEvent = PeriphEventHdl;

  Serial.println(F("Starting Firmware"));
  onConnect = WiFi.onStationModeConnected([&](const WiFiEventStationModeConnected& evt){
    deviceState::getInstance()->wifiStatus = deviceState::WIFI_CONN;
    vGUIHandlePeriph(EV_WIFI, HW_CON);
    Serial.println(F("WiFi connected!"));
	  deviceState::getInstance()->ssid = initWifi();
  });

  onDisconnect = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected& evt){
    getHWStatus()->connOFF.start();
    getHWStatus()->connON.stop();
    vGUIHandlePeriph(EV_WIFI, HW_DISCON);
    Serial.println(F("WiFi disconnected!"));
    deviceState::getInstance()->mqtt().disconnect();
    deviceState::getInstance()->wifiStatus = deviceState::WIFI_DISC;
  });

  onIp = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP& evt){
    vGUIHandlePeriph(EV_WIFI, HW_GOTIP);
    Serial.println(F("WiFi got Ip!"));
	  //deviceState::getInstance()->mqtt().connect();
    deviceState::getInstance()->wifiStatus = deviceState::WIFI_GOTIP;
    deviceState::getInstance()->mqttServiceModeHost = WiFi.dnsIP(1).toString();
    if (deviceState::getInstance()->_isInSeviceMode) deviceState::getInstance()->mqttServiceMode();
    deviceState::getInstance()->mqtt().connect();
  });

  vGUIGlueInit();
  GUIInit();
  periph->powerSaveExit();

  deviceState::getInstance(); //init device state object

  //pinMode(INTR_TOUCH, INPUT_PULLUP); //should be done after TFT init!

  attachInterrupt(digitalPinToInterrupt(INTR_SYS), handleSysInt, FALLING);
  //attachInterrupt(digitalPinToInterrupt(INTR_TOUCH), handleTouchInt, FALLING);
  attachInterrupt(digitalPinToInterrupt(INTR_NFC), handleNfcInt, RISING);

  periph->vibroPulse();

  
  pinMode(INTR_NFC, INPUT_PULLUP);
  deviceState::getInstance()->ssid=initWifi();
  periph->sleepTimeout.reset();
  //deviceState::getInstance()->debug(true);
}

void loop(void) {

  auto delayTime = 60;
  if (periph->loop((digitalRead(INTR_SYS) == 0), (bool)digitalRead(INTR_NFC))) {
  	vGUIHandlePeriph(EV_HW_SYS_INT, 0);
  };

  if(periph->touch.handleTouch(true)){
    periph->sleepTimeout.reset();
    delayTime = 1;
    vGUIHandlePeriph(EV_HW_TOUCH_INT, 0);
  }

  vWindowManagerDraw();
  vGUIHandlePeriph(EV_HW_VOLTAGE_CHANGE, periph->bat.charge());
  delay(delayTime);
  Serial.print(".");
  getHWStatus()->tick();

  deviceState::getInstance()->loop();

}
