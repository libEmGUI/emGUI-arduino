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
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <Adafruit_INA219.h>

#include "src/GUI/GUI.h"
#include "src/GUI/WindowPack.h"
#include "src/FIR-filter-class/filt.h"

#include "User_Setup.h"

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

// Signal filter
Filter lpf(LPF, 51, AFE_DATA_RATE / 1000.f, 0.05);
// Current (and voltage in high side) monitor 
Adafruit_INA219 monitor;

class Touch: public TouchWrapper, public XPT2046_Touchscreen {
public:
  Touch(Adafruit_GFX &tft):
    TouchWrapper(tft),
    XPT2046_Touchscreen(TOUCH_CS){}

  void begin(){
    XPT2046_Touchscreen::begin();

    // matrixes are applied from the end (See Matrix Dot Product order)
    dumpMatrix();

    // Transformation #3
    {
      //scale to actual width/height
      float xScale = _tft.width() / scaleRes;
      float yScale = _tft.height() / scaleRes;

      float scaleMatrix[9] = {
        xScale, 0.0f, 0.f,
        0.0f, yScale, 0.f,
        0.0f, 0.0f, 1.0f
      };

      dotTransformation(scaleMatrix);
      dumpMatrix();
    }

    // Transformation #2
    {
      //rotate touch coords according to screen orientation
      float pi = acosf(-1);
      float angle = 0.f;
      float xOffs = 0.f;
      float yOffs = 0.f;
      switch (_tft.getRotation())
      {
        case 0: angle = 3.f*pi/2.f; xOffs = scaleRes; break; // angle = 270
        case 1: angle = 0.f; break;  // angle = 0
        case 2: angle = pi/2; yOffs = scaleRes; break; // angle = 90
        case 3: angle = pi; xOffs = yOffs = scaleRes; break; // angle = 180
        default: break;
      }

      float sin = sinf(angle);
      float cos = cosf(angle);

      float rotateMatrix[9] = {
        cos, sin, xOffs,
        -sin, cos, yOffs,
        0.f, 0.f, 1.f
      };

      dotTransformation(rotateMatrix);
      dumpMatrix();
    }

    // Transformation #1
    {
      //scale to square 1000x1000, eliminating offsets (calibration) and swapping x and y points

      /*
        Matrix coefficient computations
        
        xScale = touch_area_width / total_width
        yScale = touch_area_height / total_height
        xOffset = touch_area_x_offset / total_width
        yOffset = touch_area_y_offset / total_height
      */

      auto maxAdcValueX = 3641.f;
      auto maxAdcValueY = 3450.f;
      float xOffset = -80;
      float yOffset = -80;
      //scale and flip
      float w = -scaleRes; // flip x/y
      float h = scaleRes;
      float xScale = w / maxAdcValueX;
      float yScale = h / maxAdcValueY;

      // offset compensation after flip
      if(w < 0)
        xOffset = -xOffset - w;
      if(h < 0)
        yOffset = -yOffset - h;

      float scaleMatrix[9] = {
        xScale, 0.0f, xOffset,
        0.0f, yScale, yOffset,
        0.0f, 0.0f, 1.0f
      };
      dotTransformation(scaleMatrix);
      dumpMatrix();
    }
    
  }

  bool isTouched(){
    return XPT2046_Touchscreen::touched();
  };

	TouchPoint getTouchCoords(){
    auto p = XPT2046_Touchscreen::getPoint();
    return TouchPoint(p.x, p.y);
  };

  float scaleRes = 1000.f;
};

Touch touch(tft);

void setup() {
  Serial.begin(115200);

  Serial.println(F("Ready"));

  Wire.begin(SDA_PIN, SCL_PIN);

  tft.begin();
  // set rotation first!
  tft.setRotation(1);
  // init touch with transformations after screen rotation is set!
  touch.begin();
  // Setup emGUI
  vGUIGlueInit(&tft);

  // Start GUI
  GUIInit();

  // setup and start INA219 current monitor
  monitor.begin();
  monitor.setCalibration_32V_1A();
  testFillScreen();
}

// To reduce noise in signal we use filter 
void handleData(float data) {
  auto window = WindowPlot::getInstance(false);
	lpf.do_sample(data);
	if (window) window->update(data);
}

void loop() {
  touch.loop();
  // emGUI loop
  vWindowManagerDraw();

  handleData(monitor.getCurrent_mA());
  //Serial.printf("I(mA): %2f, V(mV): %2f, P(mW):%2f \n", monitor.getCurrent_mA(), monitor.getBusVoltage_V(), monitor.getPower_mW());
}
