#pragma once

#include "WindowPack.h"

class WindowMain : public DisposableWindow<WINDOW_MENU, WindowMain> {
public:
  void create() {
    vWindowSetHeader(xThis, "Main menu");
    uint8_t offset = 20;
    uint8_t btnWidth = 120;
    uint8_t btnHeight = 120;
    uint8_t row1 = offset;
    uint8_t column1 = offset;
    uint8_t column2 = EMGUI_LCD_WIDTH - offset - btnWidth;

	auto paintBtn = pxButtonCreateFromText(column1, row1, btnWidth, btnHeight, "Paint", xThis);
	vButtonSetOnClickHandler(paintBtn,
		[](xWidget *) {
      WindowPaint::getInstance()->open();
      return true;
    });
  auto plotBtn = pxButtonCreateFromText(column2, row1, btnWidth, btnHeight, "Plot", xThis);
  vButtonSetOnClickHandler(plotBtn, 
    [](xWidget *) {
      WindowPlot::getInstance()->open();
      return true;
    });
  }

  bool onCloseRequest() {
    return false; //allow window close
  }
};