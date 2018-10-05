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

#include "src/GUI/GUI.h"
#include "Peripherial.h"
#include "emGUIGlue.h"

#include "src/GUI/deviceState.hpp"
#include "src/GUI/WindowPack.h"


#include "src/TouchWrapper/TouchWrapper.h"
#include "src/common/I2CBitbanger.h"
#include "src/common/Timeout.hpp"
#include "src/common/CommonWifi.h" 

#include "src/FIR-filter-class/filt.h"
#include "src/IIR/IIR_filter.h" 
#include <Adafruit_INA219.h>

static Filter lpf(LPF, 51, AFE_DATA_RATE / 1000.f, 0.05);
static IIR_filter iir_f(1.f / AFE_DATA_RATE);
Adafruit_INA219 monitor;
extern "C" {
#include "user_interface.h"
}

#include "User_Setup.h"


/************ Global vars and constants ******************************/
static Peripherial* periph = Peripherial::getInstance();                    // port extender (buttons, vibro, charger status)

WiFiEventHandler onConnect, onDisconnect, onIp;

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
  // Init serial and start firmware
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
  Serial.println(ESP.getCoreVersion());

  Serial.print(F("SDK: "));
  Serial.println(ESP.getSdkVersion());

  SPIFFS.begin();

  I2CBitbanger(SDA_PIN, SCK_PIN);
  periph->init();
  periph->onEvent = PeriphEventHdl;

  Serial.println(F("Starting Firmware"));

  // Set callbacks for WiFi
  onConnect = WiFi.onStationModeConnected([&](const WiFiEventStationModeConnected& evt){
    deviceState::getInstance()->wifiStatus = deviceState::WIFI_CONN;
    vGUIHandlePeriph(EV_WIFI, HW_CON);
    Serial.println(F("WiFi connected!"));
	  deviceState::getInstance()->ssid = initWifi();
  });

  onDisconnect = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected& evt){
    vGUIHandlePeriph(EV_WIFI, HW_DISCON);
    Serial.println(F("WiFi disconnected!"));
    deviceState::getInstance()->wifiStatus = deviceState::WIFI_DISC;
  });

  onIp = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP& evt){
    vGUIHandlePeriph(EV_WIFI, HW_GOTIP);
    Serial.println(F("WiFi got Ip!"));
    deviceState::getInstance()->wifiStatus = deviceState::WIFI_GOTIP;
  });

  // Setup emGUI
  vGUIGlueInit();
  // Start GUI
  GUIInit();

  deviceState::getInstance(); //init device state object, read configs and set callbacks
  deviceState::getInstance()->ssid=initWifi(); // init wifi connection
  monitor.begin();
  monitor.setCalibration_32V_1A();
}

void handleData(float data) {
  auto window = WindowPlot::getInstance(false);
	auto fData = lpf.do_sample(data);
	if (window) window->update(data);
}

void loop() {
  auto delayTime = 60;
  if (periph->loop()) {
  	vGUIHandlePeriph(EV_HW_SYS_INT, 0);
  };

  static int a = 100;
  if(periph->touch.handleTouch(true)){
    delayTime = 1;
    vGUIHandlePeriph(EV_HW_TOUCH_INT, 0);
  }

  // emGUI loop
  vWindowManagerDraw();
  

  deviceState::getInstance()->loop();
    if (a>=250 ) a =0;
    else a++;
    handleData(monitor.getCurrent_mA());
  //handleData(a);

  if (!skipDelay) {
    delay(delayTime);
    Serial.print(".");
  } else {
    Serial.printf("I(mA): %2f, V(mV): %2f, P(mW):%2f \n",monitor.getCurrent_mA(), monitor.getBusVoltage_V(), monitor.getPower_mW());
  }
}
