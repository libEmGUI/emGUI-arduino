#include "GUI.h"
#include <vector>
#include <functional>
#include <memory>

using namespace emGUI;

static xStatusBar * statusbar;
static WindowHeader::uniquePtr header;
static xButton * crossButton;

bool MainWindowCloseRequestHdl(xWidget *);

// Action on interface creating
// Init statusbar and open main window
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

  // You should use R5G6B5 bmp (just put it in SPIFFS) I prefer to use GIMP to convert bmp. 
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

void GUIInit() {
	pxWindowManagerCreate(bGUIonWindowManagerCreateHandler);
}
