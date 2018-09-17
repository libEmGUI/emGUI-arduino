#include "GUI.h"

#include <ESP8266WiFi.h>
#include <vector>
#include <functional>
#include <memory>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "src/DummyGUI/WindowPack.h"

static Peripherial* periph = Peripherial::getInstance();

using namespace emGUI;

static xStatusBar * statusbar;
static WindowHeader::uniquePtr header;
static WiFiWidget::uniquePtr wifiWidget;
static xPlotData_t plotLead;
xPlotData_t * pxGUIGetPlotData() {
	return &plotLead;
}
static xLabel * currentMonitor;
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
  ButtonToggle::uniquePtr wifi;
 
  String mac = WiFi.macAddress();


  void create() {
    if (deviceState::getInstance()->serviceMode){
      bWidgetSetBgPicture(xThis, "/service.bmp");
    }
    vWindowSetHeader(xThis, "Главное Меню");
    uint8_t offset = 10;

    uint8_t row1 = offset;
    uint8_t row2 = row1 + 80 + offset;
	uint8_t row3 = row2 + 70;
    uint8_t column1 = offset;
    uint8_t column2 = EMGUI_LCD_WIDTH / 2 - 30;
    uint8_t column3 = EMGUI_LCD_WIDTH - offset - 60;

  auto menuBut = pxButtonCreateFromText(column3, row1, 60, 60, "plot", xThis);
    vButtonSetOnClickHandler(menuBut, 
      [](xWidget *) {
      vWindowManagerOpenWindow(WINDOW_PLOT);
      return true;
    });
  auto window_show_ampermeter = pxWindowCreate(WINDOW_PLOT);

  vWindowSetHeader(window_show_ampermeter, "Plot, mA");

  plotLead.bDataFilled = false;
  plotLead.bWriteEnabled = false;
  plotLead.sDataDCOffset = -500;
  plotLead.sName = "Test";
  plotLead.ulElemCount = AFE_DATA_RATE * 10;
  plotLead.psData = (short *)malloc(sizeof(*plotLead.psData)*plotLead.ulElemCount);
  plotLead.ulWritePos = 0;

  xPlot * plot = pxPlotCreate(0, 0, EMGUI_LCD_WIDTH, EMGUI_LCD_HEIGHT - EMGUI_STATUS_BAR_HEIGHT -20, window_show_ampermeter, &plotLead);
  vPlotSetScale(plot, 250);
  currentMonitor = pxLabelCreate(10, EMGUI_LCD_HEIGHT - EMGUI_STATUS_BAR_HEIGHT - 20, EMGUI_LCD_WIDTH, 20, "I: _ (0.1 mA)", xGetDefaultFont(), 100, window_show_ampermeter);
  vLabelSetTextAlign(currentMonitor, LABEL_ALIGN_CENTER);
  vLabelSetVerticalAlign(currentMonitor, LABEL_ALIGN_MIDDLE);
  
	auto btn2 = pxButtonCreateFromText(column2, row1, 60, 60, "0", xThis);
	vButtonSetOnClickHandler(btn2,
		[](xWidget *) {
		WindowIDLE::getInstance()->open();
		return true;
	});
	auto btn3 = pxButtonCreateFromText(column1, row1, 60, 60, "Paint", xThis);
	vButtonSetOnClickHandler(btn3,
		[](xWidget *) {
		TPTest::getInstance()->open();
		return true;
	});


  auto btn7 = pxButtonCreateFromText(column1, row3, 60, 30, "update", xThis);
  vButtonSetOnClickHandler(btn7,
    [](xWidget *) {
    t_httpUpdate_return ret = ESPhttpUpdate.update("http://ci-test.flexibity.com/firmware.bin");
    switch(ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
      }
      return true;

  });

  auto btn9 = pxButtonCreateFromText(column3, row3, 60, 30, "spiffs", xThis);
  vButtonSetOnClickHandler(btn9,
	  [](xWidget *) {
	  t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs("http://ci-test.flexibity.com/spiffs.bin", "");
	  switch (ret) {
	  case HTTP_UPDATE_FAILED:
		  Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
		  break;

	  case HTTP_UPDATE_NO_UPDATES:
		  Serial.println("HTTP_UPDATE_NO_UPDATES");
		  break;

	  case HTTP_UPDATE_OK:
		  Serial.println("HTTP_UPDATE_OK");
		  break;
	  }
	  return true;


    
    
    return true;
  });


  }



  bool onCloseRequest() {
    auto dial = iModalDialogOpen(0, "ny", "Close?", "You are about to close main app! Are you sure?");

    vModalDialogSetHandler(dial, NULL, [](char cBtn, void* ctx) -> bool {
		(void)ctx;
      if (cBtn == 'y'){
        //TODO: poweroff
      }
      return true; // dialog can close on this button
    });
    return true; //allow window close
  }
};


  void vGUIUpdateCurrentMonitor() {
    auto data = plotLead.psData[plotLead.ulWritePos];
    //iLabelPrintf(currentMonitor, "I_Avg: %.1f; I: %d.%d (mA)", extraP.averageCurrent / 10.f, data / 10, abs(data % 10));
  }

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

  auto usY = 2;
  auto usW = usWidgetGetH(statusbar) - usY * 2;
  auto usX = usWidgetGetW(statusbar) - usWidgetGetH(statusbar);

  usX = usWidgetGetW(statusbar) - pxDrawHDL()->usGetPictureW("/batch/0.bmp") - 5;
  usY = usWidgetGetH(statusbar) / 2 - pxDrawHDL()->usGetPictureH("/batch/0.bmp") / 2 - 1;

  usY = (usWidgetGetH(statusbar) - pxDrawHDL()->usFontGetH(getSmallFont())) / 2;
  usW = usWidgetGetW(statusbar) * 50 / 100;
  usX = usWidgetGetW(statusbar) / 2 - usW / 2;

  header = std::make_unique<WindowHeader>(usX, usY, usW, 0, getSmallFont(), EMGUI_WINDOW_HEADER_LENGTH, statusbar);

  usX = 5;
  usY = usWidgetGetH(statusbar) / 2 - pxDrawHDL()->usGetPictureH("/wf/0.bmp") / 2 - 9;
  wifiWidget = std::make_unique<WiFiWidget>(usX, usY, statusbar);

  WindowMain::getInstance()->open();
  return true;
}

void GUIInit(){
	pxWindowManagerCreate(bGUIonWindowManagerCreateHandler);
}

void vGUIHandlePeriph(eHW event, int32_t param1) {

}
