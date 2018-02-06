#include "GUI.h"

#include <vector>

using namespace emGUI;

bool MainWindowCloseRequestHdl(xWidget *);

class TPTest: public DisposableWindow<WINDOW_TP_TEST, TPTest> {
public:
  void create() {
    
    vWindowSetFullScreen(xThis, true);

    vWidgetSetClickable(xThis, true);
    xThis->pxCheckTSRoutine = [] (xWidget *pxW, xTouchEvent * xEv) -> bool {
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
  int colorNum = 0;
};


class WindowHWTest: public emGUI::DisposableWindow<WINDOW_HW_TEST, WindowHWTest> {
public:
    
  void create() {
    
    vWindowSetHeader(xThis, "HW Test");

    auto spacing = 2;
    auto h = 10;
    auto y = spacing / 2;
    auto x = 5;
    auto w = 130;
    auto len = 20;

    l1 = pxLabelCreate(x, y, w, 110, F_WRP(""), NULL, 100, xThis);

    bLabelSetMultiline(l1, true);
    y += usWidgetGetH(l1) + spacing;

    l2 = pxLabelCreate(x, y, 230, 50, F_WRP(""), NULL, 100, xThis);
    bLabelSetMultiline(l2, true);

    x = 150;
    y = 10;
    w = 80;
    h = 40;
    spacing = 10;
    
    draw = std::make_unique<Button>( x, y, w, h, "DRAW", this );
    draw->onClick = [this](Button* b) {
      this->close();
      TPTest::getInstance()->open();
    };

    y += draw->H() + spacing;
    tp = std::make_unique<ButtonToggle>(x, y, w, h, "TP ON", this);
    tp->onToggle = [](ButtonToggle* b, bool state) {
      //do something
    };

    y += tp->H() + spacing;
    x = 5;
    y = usWidgetGetY1(l2, false);
    w = 75;
    spacing = 2;

    act = std::make_unique<ButtonToggle>(x, y, w, h, "ACTIVE", this);
    act->onToggle = [](ButtonToggle* b, bool state) {
      //do something
    };

    x += act->W() + spacing;
    vib = std::make_unique<ButtonToggle>(x, y, w, h, "VBR", this);
    vib->onToggle = [](ButtonToggle* b, bool state) {
      //do something
    };

    x += vib->W() + spacing;
    bl = std::make_unique<ButtonToggle>(x, y, w, h, "BL", this);
    bl->onToggle = [](ButtonToggle* b, bool state) {
      //do something
    };

    x += bl->W() + spacing;
    firstStart = true;
  }

  bool onDrawUpdate() {

    firstStart = false;

    return false;
  }

  void resetToggles(){
    tp->state(false);
    act->state(false);
    bl->state(false);
    vib->state(false);
  }

  bool onKeypress(uint16_t uEv) {
    if (uEv == 1) {
      resetToggles();
      return true;
    }

    return true;
  }

  bool onClose(){
    resetToggles();
    return true;
  }

  bool onOpen(){
    resetToggles();
    return true;
  }
private:

  Button::uniquePtr draw;
  ButtonToggle::uniquePtr tp;
  ButtonToggle::uniquePtr act;
  ButtonToggle::uniquePtr bl;
  ButtonToggle::uniquePtr vib;
  xLabel *l1;
  xLabel *l2;
  bool firstStart;
};

class WindowMain : public DisposableWindow<WINDOW_MENU, WindowMain> {
public:
  ButtonToggle::uniquePtr wifi;
  void create() {
    vWindowSetHeader(xThis, "Main menu");
    uint8_t offset = 10;

    uint8_t row1 = offset;
    uint8_t row2 = row1 + 80 + offset;
    uint8_t column1 = offset;
    uint8_t column2 = EMGUI_LCD_WIDTH / 2 - 30;
    uint8_t column3 = EMGUI_LCD_WIDTH - offset - 60;
    auto btn = pxButtonCreateFromImageWithText(column1, row1, "/about.bmp", "HW Test", xThis);
    vButtonSetOnClickHandler(btn,
      [](xWidget *) {
      WindowHWTest::getInstance()->open();
      return true;
    });
    auto btn2 = pxButtonCreateFromImageWithText(column2, row1, "/magic.bmp", "Battery", xThis);
    vButtonSetOnClickHandler(btn2,
      [](xWidget *) {
      return true;
    });

    auto btn3 = pxButtonCreateFromText(column1, row2, 60, 60, "log", xThis);
    vButtonSetOnClickHandler(btn3,
      [](xWidget *) {
      return true;
    });

    auto btn4 = pxButtonCreateFromText(column2, row2, 60,60, "wifi", xThis);
    vButtonSetOnClickHandler(btn4,
      [](xWidget *) {
        return true;
    });
  }

  bool onCloseRequest() {
    auto dial = iModalDialogOpen(0, "ny", "Close?", "You are about to close main app! Are you sure?");

    vModalDialogSetHandler(dial, NULL, [](char cBtn, void* ctx) -> bool {

      if (cBtn == 'y'){
        //power off device
      }

      return true; // dialog can close on this button
    });

    return false; //suppress w1 close!
  }
};


// Action on interface creatings
bool bGUIonWindowManagerCreateHandler(xWidget *) {
  vWindowManagerSetKeypressHandler([](int iID, uint16_t uEv){
    if(uEv = 1)
      vWindowManagerCloseActiveWindow();

    return true;
  });
  WindowMain::getInstance()->open();
  return true;
}

void GUIInit(){
  pxWindowManagerCreate(bGUIonWindowManagerCreateHandler);
}

