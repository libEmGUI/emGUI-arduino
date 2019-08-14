#include <Arduino.h>
#include <emGUI.h>
#include "src/GUI/GUI.h"
#include "src/TouchWrapper/TouchWrapper.h"

#include <FS.h>

TouchWrapper touch;

#include "User_Setup.h"

#include "SPI.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(ILI9341_BLACK);
  yield();
  tft.fillScreen(ILI9341_RED);
  yield();
  tft.fillScreen(ILI9341_GREEN);
  yield();
  tft.fillScreen(ILI9341_BLUE);
  yield();
  tft.fillScreen(ILI9341_BLACK);
  yield();
  return micros() - start;
}

void setup() {
  Serial1.begin(115200);
  touch.init();
  tft.begin();
  tft.setRotation(2);
  vGUIGlueInit(&tft);
  GUIInit();
  testFillScreen();
}

void loop(void) {
  vWindowManagerDraw();
  touch.handleTouch(true);
}
