
#pragma once

#include "WindowPack.h"

class WindowPaint: public DisposableWindow<WINDOW_PAINT, WindowPaint> {
public:
  void create() {
    
    vWindowSetFullScreen(xThis, true);

    vWidgetSetClickable(xThis, true);
    xThis->pxCheckTSRoutine = [] (xWidget *pxW, xTouchEvent_t * xEv) -> bool {
      auto x = xEv->x;
      auto y = xEv->y;
      auto rectSize = 2;
      x = (x >= rectSize)?x-rectSize:x;
      y = (y >= rectSize)?y-rectSize:y;
      pxDrawHDL()->vRectangle(x, y, x + rectSize, y + rectSize, 0, true);
      return false;
    };
  }
  bool onDrawUpdate() { 
    pxDrawHDL()->vHLine(0, EMGUI_LCD_HEIGHT/2, EMGUI_LCD_WIDTH, EMGUI_COLOR_GRAY);
    pxDrawHDL()->vVLine(EMGUI_LCD_WIDTH/2, 0,  EMGUI_LCD_HEIGHT, EMGUI_COLOR_GRAY);   
    return true; 
  }
  uint16_t prvGetNextColor() {
    static std::vector<uint16_t> colors = {
      EMGUI_COLOR_WHITE,
      EMGUI_COLOR_RED,
      EMGUI_COLOR_GREEN,
      EMGUI_COLOR_BLUE,
      EMGUI_COLOR_BLACK

    };
    
    if (++colorNum >= colors.size())
      colorNum = 0;
    return colors[colorNum];
  }

    bool onKeypress(uint16_t uEv) {
    if (uEv == 0) {
      vWidgetSetBgColor(xThis, prvGetNextColor(), false);
      return true;
    }

    return false;
  }

protected:
  unsigned int colorNum = 0;
};
