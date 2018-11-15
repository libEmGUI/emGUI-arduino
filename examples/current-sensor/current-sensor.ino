/*
*  main file 
*  
*  TFT configs are in User_Setup.h
*  Flexibity 2017 // Roman Savrulin, Mikhail Natalenko
*/

#include <Wire.h>
#include <Hash.h>
#include <FS.h>

#include "src/GUI/GUI.h"
#include "emGUIGlue.h"
#include "src/GUI/WindowPack.h"

#include "src/TouchWrapper/TouchWrapper.h"
#include "src/common/I2CBitbanger.h"
#include "src/common/Timeout.hpp" 
#include <Adafruit_INA219.h>
#include "src/FIR-filter-class/filt.h"
// #include "src/IIR/IIR_filter.h" 

extern "C" {
  #include "user_interface.h"
}
#include "User_Setup.h"

Filter lpf(LPF, 51, AFE_DATA_RATE / 1000.f, 0.05);
// IIR_filter iir_f(1.f / AFE_DATA_RATE);
Adafruit_INA219 monitor;
TouchWrapper touch;

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

  Serial.println(F("Ready"));
  SPIFFS.begin();

  I2CBitbanger(SDA_PIN, SCK_PIN);
  touch.init();

  // Setup emGUI
  vGUIGlueInit();
  // Start GUI
  GUIInit();

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

  touch.handleTouch(true);

  // emGUI loop
  vWindowManagerDraw();

  handleData(monitor.getCurrent_mA());

  if (!skipDelay) {
    delay(delayTime);
    Serial.print(".");
  } else {
    Serial.printf("I(mA): %2f, V(mV): %2f, P(mW):%2f \n", monitor.getCurrent_mA(), monitor.getBusVoltage_V(), monitor.getPower_mW());
  }
}
