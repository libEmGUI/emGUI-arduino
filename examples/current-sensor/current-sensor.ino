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

#include "GUI.h"
#include "Peripherial.h"
#include "emGUIGlue.h"

#include "src/DummyGUI/deviceState.hpp"
#include "src/DummyGUI/WindowPack.h"


#include "src/TouchWrapper/TouchWrapper.h"
#include "src/common/I2CBitbanger.h"
#include "src/common/Timeout.hpp"
#include "src/common/CommonWifi.h" 

#include "src/FIR-filter-class/filt.h"
#include "src/IIR/IIR_filter.h" 


static Filter lpf(LPF, 51, AFE_DATA_RATE / 1000.f, 0.05);
static IIR_filter iir_f(1.f / AFE_DATA_RATE);

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

}

void handleData(int data, xPlotData_t * buffer) {
	auto pd = buffer;

	//logger->info(data);
	auto fData = (int16_t)(lpf.do_sample(data));
	pd->psData[pd->ulWritePos] = fData;

	//params->extraParams->averageCurrent = (float)iir_f.do_sample(fData);
	vGUIUpdateCurrentMonitor();
	pd->ulWritePos++;
	if (pd->ulWritePos >= pd->ulElemCount) {
		pd->ulWritePos = 0;
		pd->bDataFilled = true;
	}
}

xPlotData_t *  plotData= pxGUIGetPlotData();
void loop(void) {

  auto delayTime = 0;
  if (periph->loop()) {
  	vGUIHandlePeriph(EV_HW_SYS_INT, 0);
  };

  if(periph->touch.handleTouch(true)){
    delayTime = 1;
    vGUIHandlePeriph(EV_HW_TOUCH_INT, 0);
  }

  // emGUI loop
  vWindowManagerDraw();
  delay(delayTime);
  Serial.print(".");

  deviceState::getInstance()->loop();
  static float arg = 0.f;

  arg += 3.14f / 1000;
  handleData(int(sin(arg) * 100.f) + 100, plotData);
}
