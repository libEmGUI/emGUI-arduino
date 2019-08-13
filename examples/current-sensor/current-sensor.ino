/*
*  main file 
*  
*  TFT configs are in User_Setup.h
*  fbits.tech 2018 // Roman Savrulin, Mikhail Natalenko
*/
#include <Arduino.h>
#include <Wire.h>
#include <Hash.h>
#include <FS.h>

#include "src/GUI/GUI.h"
#include "src/GUI/WindowPack.h"
#include "src/TouchWrapper/TouchWrapper.h"
#include <Adafruit_INA219.h>
#include "src/FIR-filter-class/filt.h"

#include "User_Setup.h"

// Signal filter
Filter lpf(LPF, 51, AFE_DATA_RATE / 1000.f, 0.05);
// Current (and voltage in high side) monitor 
Adafruit_INA219 monitor;
TouchWrapper touch;

void setup() {
  Serial.begin(115200);

  Serial.println(F("Ready"));

  Wire.begin(SDA_PIN, SCL_PIN);
  touch.init();

  // Setup emGUI
  vGUIGlueInit();
  // Start GUI
  GUIInit();

  // setup and start INA219 current monitor
  monitor.begin();
  monitor.setCalibration_32V_1A();
}

// To reduce noise in signal we use filter 
void handleData(float data) {
  auto window = WindowPlot::getInstance(false);
	lpf.do_sample(data);
	if (window) window->update(data);
}

void loop() {
  touch.handleTouch(true);
  // emGUI loop
  vWindowManagerDraw();

  handleData(monitor.getCurrent_mA());
  Serial.printf("I(mA): %2f, V(mV): %2f, P(mW):%2f \n", monitor.getCurrent_mA(), monitor.getBusVoltage_V(), monitor.getPower_mW());
}
