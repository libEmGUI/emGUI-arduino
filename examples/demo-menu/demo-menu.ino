#include <Arduino.h>
#include <emGUI.h>
#include "emGUIGlue.h"
#include "src/GUI/GUI.h"
#include "src/TouchWrapper/TouchWrapper.h"

#include <FS.h>

TouchWrapper touch;

void setup() {
  Serial1.begin(115200);
  touch.init();
  vGUIGlueInit();
  GUIInit();
}

void loop(void) {
  vWindowManagerDraw();
  touch.handleTouch(true);
}
