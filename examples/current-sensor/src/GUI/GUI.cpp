#include "GUI.h"
#include <vector>
#include <functional>
#include <memory>
#include "WindowPack.h"
using namespace emGUI;

static xStatusBar * statusbar;
static WindowHeader::uniquePtr header;
static xButton * crossButton;


bool MainWindowCloseRequestHdl(xWidget *);

class TPTest: public DisposableWindow<WINDOW_TP_TEST, TPTest> {
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
      TPTest::getInstance()->open();
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

// Action on interface creatings
bool bGUIonWindowManagerCreateHandler(xWidget *) {
  vWindowManagerSetKeypressHandler([](int iID, uint16_t uEv){
	  (void)iID;
    if(uEv = 1)
      vWindowManagerCloseActiveWindow();

    return true;
  });

  //Status bar 
  statusbar = pxWindowManagerGetStatusBar();
  auto usY = (usWidgetGetH(statusbar) - pxDrawHDL()->usFontGetH(getSmallFont())) / 2;
  auto usW = usWidgetGetW(statusbar) * 50 / 100;
  auto usX = usWidgetGetW(statusbar) / 2 - usW / 2;

  header = std::make_unique<WindowHeader>(usX, usY, usW, 0, getSmallFont(), EMGUI_WINDOW_HEADER_LENGTH, statusbar);

  usX = usWidgetGetW(statusbar) - pxDrawHDL()->usGetPictureW("/cross.bmp") - 1 ;
  usY = 1;

  crossButton = pxButtonCreateFromImage(usX, usY, "/cross.bmp", statusbar);
  vButtonSetOnClickHandler((xWidget*)crossButton, 
    [](xWidget *) {
      vWindowManagerCloseActiveWindow();
      return true;
  });

  WindowMain::getInstance()->open();
  return true;
}

void GUIInit(){
	pxWindowManagerCreate(bGUIonWindowManagerCreateHandler);
}

void vGUIHandlePeriph(eHW event, int32_t param1) {

}
