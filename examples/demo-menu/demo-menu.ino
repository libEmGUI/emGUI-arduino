#include <Arduino.h>
#include <emGUI.h>
#include "emGUIGlue.h"
#include "Gui.h"
#include "src/TouchWrapper/TouchWrapper.h"

#include "User_Setup.h"

#include <Wire.h>
#include <FS.h>

TouchWrapper touch;

void setup() {
  SPIFFS.begin();

  Wire.begin(SDA_PIN, SCK_PIN);
  touch.init();
  vGUIGlueInit();
  GUIInit();
}

void loop(void) {
  vWindowManagerDraw();
  touch.handleTouch(true);
}
 
