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
    XPT2046_Touchscreen(TOUCH_CS){
      xOffset = -330;
      yOffset = -330;
      xScale = 1.f;
      yScale = 1.f;
      xMax = 3600;
      yMax = 3500;
    }

  bool isTouched(){
    return XPT2046_Touchscreen::touched();
  };

	TouchPoint getTouchCoords(){
    auto p = XPT2046_Touchscreen::getPoint();
    TouchWrapper::TouchPoint tp(p.x, p.y);    
    
    /*
    c0 = touch_area_width / total_width
    c2 = touch_area_height / total_height
    c1 = touch_area_x_offset / total_width
    c3 = touch_area_y_offset / total_height
    */

    float sin = 0.f;
    float cos = 1.f;
    float wc = 0.f;
    float hc = 0.f;

    switch (_tft.getRotation())
    {
    case 0:
      wc = 1.f;
      hc = 0.f;
      sin = -1.f;
      cos = 0.f;
    case 1:
      sin = 0.f;
      cos = 1.f;
      break;
    case 3:
      wc = 1.f;
      hc = 1.f;
      sin = 0.f;
      cos = -1.f;
    case 2:
      wc = 0.f;
      hc = 1.f;
      sin = 1.f;
      cos = 0.f;
      break;
    default:
      break;
    }

    auto scaleRes = 1000.f;
    //scale to square 1000x1000, eliminating offsets
    auto maxAdcValueX = 3641.f;
    auto maxAdcValueY = 3450.f;
    auto xOffset = -80;
    auto yOffset = -80;
    //scale and flip
    float w = -scaleRes;
    float h = scaleRes;
    float c0 = w / maxAdcValueX;
    float c2 = h / maxAdcValueY;
    float c1 = xOffset;
    float c3 = yOffset;

    if(w < 0)
      c1 = -c1 - w;
    if(h < 0)
      c3 = -c3 - h;

    float scaleMatrix[9] = {
      c0, 0.0f, c1,
      0.0f, c2, c3,
      0.0f, 0.0f, 1.0f
    };

    auto scale = tp.transform(scaleMatrix, 9);

    /*float rotateMatrix[9] = {
      cos, sin, wc * maxAdcValue,
      -sin, cos, hc * maxAdcValue,
      0, 0, 1
    };*/

    float c4 = 0.f;
    float c5 = 0.f;
    c1 = 0.f;
    c3 = 0.f;
    //replace this to sinus/cosinus functions!
    switch (_tft.getRotation())
    {
    case 0: // angle = 270
      c0 = 0;
      c2 = 0;
      c4 = -1;
      c5 = 1;
      break;
    case 1: // angle = 0
      c0 = 1;
      c2 = 1;
      break;
    case 2: // angle = 90
      c0 = 0;
      c2 = 0;
      c4 = 1;
      c5 = -1;
      break;
    case 3: // angle = 180
      c0 = -1;
      c2 = -1;
      c4 = 0;
      c5 = 0;
      break;
    default:
      c0 = 1;
      c2 = 1;
      break;
    }

    if(c0 < 0 || c4 < 0)
      c1 = scaleRes;

    if(c2 < 0 || c5 < 0)
      c3 = scaleRes;

    //c0 = c0*_tft.width() / scaleRes;
    //c2 = c2*_tft.height() / scaleRes;

    float rotateMatrix[9] = {
      c0, c4, c1,
      c5, c2, c3,
      0.f, 0.f, 1.f
    };

    auto rotate = scale.transform(rotateMatrix, 9);

    tp.x = rotate.x;
    tp.y = rotate.y;

    c0 = _tft.width() / scaleRes;
    c2 = _tft.height() / scaleRes;

    float scaleMatrix2[9] = {
      c0, 0.0f, 0.f,
      0.0f, c2, 0.f,
      0.0f, 0.0f, 1.0f
    };

    auto scale2 = rotate.transform(scaleMatrix2, 9);

    Serial.printf("W: %d; H: %d\n", _tft.width(), _tft.height());
    Serial.printf("x: %d; y: %d Raw\n", p.x, p.y);
    Serial.printf("x: %d; y: %d Scaled\n", scale.x, scale.y);
    Serial.printf("x: %d; y: %d Rot\n", rotate.x, rotate.y);
    Serial.printf("x: %d; y: %d Scale2\n", scale2.x, scale2.y);
    
    return scale2;
  };
};

Touch touch(tft);

void setup() {
  Serial.begin(115200);

  Serial.println(F("Ready"));

  Wire.begin(SDA_PIN, SCL_PIN);
  touch.begin();

  tft.begin();
  tft.setRotation(1); //0 rotation doesn't work???
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
