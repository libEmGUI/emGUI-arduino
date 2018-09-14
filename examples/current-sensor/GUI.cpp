#include "GUI.h"

#include <ESP8266WiFi.h>
#include <vector>
#include <functional>
#include <memory>
#include "CommonWifi.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "src/DummyGUI/WindowPack.h"

static Peripherial* periph = Peripherial::getInstance();

using namespace emGUI;

static HWStatus hwst;
static xStatusBar * statusbar;
static WindowHeader::uniquePtr header;
static BatteryWidget::uniquePtr batWidget;
static WiFiWidget::uniquePtr wifiWidget;
static MQTTWidget::uniquePtr mqttWidget;
void batteryLevelWarnings(int level) {
	static bool last = false;
	bool now = level < 30;

	if (last != now) {
		if (now) {
			WindowMessage::getInstance()->open("Батарея садится", "Поставьте браслет на зарядку");
		}
		else {
			WindowMessage::getInstance()->close();
		}
	}
	last = now;
}

HWStatus * getHWStatus() {
  return &hwst;
}

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


class WindowWIFI: public DisposableWindow<WINDOW_WIFI, WindowWIFI> {
public:
  void create() {
    vWindowSetHeader(xThis, "WiFi Monitor");
    auto len = 250;

    l1 = pxLabelCreate(0, 0, 200, 200, F_WRP(""), NULL, 100, xThis);
    bLabelSetMultiline(l1, true);
  }

  bool onDrawUpdate() {
    auto pxS = &hwst;
    if(firstStart ||
       pxS->connONTime ||
       pxS->connOFFTime ||
       pxS->connONi ||
       pxS->connOFFi ||
       pxS->connIPi )
      iLabelPrintf(l1,
      "on (ms): %d \n"
      "off (ms): %d \n"
      "-------\n"
      "status: %d \n"
      "ON count: %d \n"
      "OFF count: %d \n"
      "IP count: %d \n"
      ,
      pxS->connONTime.v(),
      pxS->connOFFTime.v(),
      pxS->WiFiStatus.v(),
      pxS->connONi.v(),
      pxS->connOFFi.v(),
      pxS->connIPi.v()
    );
    firstStart = false;
	return false;
  }

protected:
bool firstStart;
  xLabel *l1;
};


class WindowBattery: public emGUI::DisposableWindow<WINDOW_BATTERY, WindowBattery> {
public:
  void create() {
    vWindowSetHeader(xThis, "Battery Monitor");

    auto len = 250;
    l1 = pxLabelCreate(0, 0, 180, 200, F_WRP(""), NULL, 100, xThis);
    bLabelSetMultiline(l1, true);

    doCalib = std::make_unique<ButtonToggle>(180, 105, 60,40, "ST", this);
    doCalib->state(!Peripherial::getInstance()->bat.hasCalib());
    doCalib->onToggle = [](ButtonToggle* b, bool state) -> bool {
		
		if (state) {
			Peripherial::getInstance()->bat.resetCalib();
			return true;
		}
	
        return false;
    };
  }
  bool onDrawUpdate() {
    auto pxS = &hwst;

	auto &bat = Peripherial::getInstance()->bat;

	if (doCalib->state() != !bat.hasCalib()) {
		doCalib->state(!bat.hasCalib());
	}
    
    if(firstStart ||
		bat.adcValue ||
       pxS->chargeTime ||
       bat.status ||
       pxS->dischargeTime)
      iLabelPrintf(l1,
	  "ADC: %d; v: %d  : %d %%\n"
      "status: %d \n"
      "time (ch): %s s \n"
      "time (dh): %s s \n"
      "calib: %d \n"
      ,
      bat.adcValue.v(),
	  bat.voltage.v(),
	  bat.charge(),
	  bat.status.v(),

      pxS->chargeSW.getFormatedResult(),
      pxS->dischargeSW.getFormatedResult(),
      periph->bat.ratio
    );

	pxS->chargeTime.v();
	pxS->dischargeTime.v();

    firstStart = false;
	return false;
  }
  bool onClose(){
    firstStart = true;
    return true;
  }

  bool onOpen(){
    firstStart = true;
	return true;
  }

private:
  ButtonToggle::uniquePtr CH;
  ButtonToggle::uniquePtr STB;
  ButtonToggle::uniquePtr doCalib;
  ButtonToggle::uniquePtr resetCalib;
  bool firstStart;
  xLabel *l1;
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
		(void)b;
      this->close();
      TPTest::getInstance()->open();
    };

    y += draw->H() + spacing;
    tp = std::make_unique<ButtonToggle>(x, y, w, h, "TP ON", this);
    tp->onToggle = [](ButtonToggle* b, bool state) -> bool {
		(void)b;
      Peripherial::getInstance()->touchPower(!state);
	  return true;
    };

    y += tp->H() + spacing;
    x = 5;
    y = usWidgetGetY1(l2, false);
    w = 75;
    spacing = 2;

    act = std::make_unique<ButtonToggle>(x, y, w, h, "ACTIVE", this);
    act->onToggle = [](ButtonToggle* b, bool state) -> bool {
		(void)b;
      Peripherial::getInstance()->dcdcActive(!state);
	  return true;
    };

    x += act->W() + spacing;
    vib = std::make_unique<ButtonToggle>(x, y, w, h, "VBR", this);
    vib->onToggle = [](ButtonToggle* b, bool state) -> bool {
		(void)b;
      Peripherial::getInstance()->vibro(state);
	  return true;
    };

    x += vib->W() + spacing;
    bl = std::make_unique<ButtonToggle>(x, y, w, h, "BL", this);
    bl->onToggle = [](ButtonToggle* b, bool state) -> bool {
		(void)b;
      Peripherial::getInstance()->backlight(!state);
	  return true;
    };

    x += bl->W() + spacing;
    firstStart = true;
  }

  bool onDrawUpdate() {
    auto pxS = &hwst;
	auto &bat = Peripherial::getInstance()->bat;
    if(pxS->NFC ||
      pxS->b1 || 
      pxS->b2 ||
      pxS->SysINT ||
      pxS->TouchINT ||
      pxS->ps||
      firstStart)
      iLabelPrintf(l1,
        "NFC: %d\n"
        "Button: %d/%d\n"
        "INTs: %d/%d\n"
        "PS: %s\n"
        "UART: Y",
        pxS->NFC.v(),
        pxS->b1.v(),
        pxS->b2.v(),
        pxS->SysINT.v(),
        pxS->TouchINT.v(),
        (pxS->ps.v()) ? "FAR" : "CLOSE"
      );

    if (bat.status ||
      bat.voltage ||
      pxS->WiFiStatus ||
      firstStart)
    iLabelPrintf(l2,
      "BAT: %d/%dmV\n"
      "WiFi \"%s\", %d",
		bat.status.v(),
		bat.voltage.v(),
      deviceState::getInstance()->ssid.c_str(),
      pxS->WiFiStatus.v()
    );
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
    periph->enableSleep(true);
    resetToggles();
    return true;
  }

  bool onOpen(){
	periph->enableSleep(false);
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
    auto btn = pxButtonCreateFromText(column1, row1, 60, 60,"HW Test", xThis);
    vButtonSetOnClickHandler(btn,
      [](xWidget *) {
      WindowHWTest::getInstance()->open();
	  //WindowBattery::getInstance()->open();
      return true;
    });
	auto btn2 = pxButtonCreateFromText(column2, row1, 60, 60, "0", xThis);
	vButtonSetOnClickHandler(btn2,
		[](xWidget *) {
		WindowIDLE::getInstance()->open();
		return true;
	});
	auto btn3 = pxButtonCreateFromText(column3, row1, 60, 60, "1", xThis);
	vButtonSetOnClickHandler(btn3,
		[](xWidget *) {
		WindowPopUp::getInstance()->open();
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

  });

    auto btn8 = pxButtonCreateFromText(column2, row2+70, 60, 30, "deep", xThis);
  vButtonSetOnClickHandler(btn8,
    [](xWidget *) {
      periph->powerTest(1000);
      return true;

  });
	auto btn5 = pxButtonCreateFromText(column1, row2, 60, 60, "3", xThis);
	vButtonSetOnClickHandler(btn5,
		[](xWidget *) {
		WindowTask::getInstance()->open();
		return true;
	});

  }

  bool onCloseRequest() {
    auto dial = iModalDialogOpen(0, "ny", "Close?", "You are about to close main app! Are you sure?");

    vModalDialogSetHandler(dial, NULL, [](char cBtn, void* ctx) -> bool {
		(void)ctx;
      if (cBtn == 'y'){
        Peripherial::getInstance()->powerOff();
      }

      return true; // dialog can close on this button
    });

    return true; //allow window close
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

  auto usY = 2;
  auto usW = usWidgetGetH(statusbar) - usY * 2;
  auto usX = usWidgetGetW(statusbar) - usWidgetGetH(statusbar);

  usX = usWidgetGetW(statusbar) - pxDrawHDL()->usGetPictureW("/batch/0.bmp") - 5;
  usY = usWidgetGetH(statusbar) / 2 - pxDrawHDL()->usGetPictureH("/batch/0.bmp") / 2 - 1;
  batWidget = std::make_unique<BatteryWidget>(usX, usY, statusbar);

  usY = (usWidgetGetH(statusbar) - pxDrawHDL()->usFontGetH(getSmallFont())) / 2;
  usW = usWidgetGetW(statusbar) * 50 / 100;
  usX = usWidgetGetW(statusbar) / 2 - usW / 2;

  header = std::make_unique<WindowHeader>(usX, usY, usW, 0, getSmallFont(), EMGUI_WINDOW_HEADER_LENGTH, statusbar);

  usX = 5;
  usY = usWidgetGetH(statusbar) / 2 - pxDrawHDL()->usGetPictureH("/wifi0.bmp") / 2 - 9;
  wifiWidget = std::make_unique<WiFiWidget>(usX, usY, statusbar);
  mqttWidget = std::make_unique<MQTTWidget>(usX+ wifiWidget->getWidth() + 2, usY, statusbar);

  WindowIDLE::getInstance()->open();
  return true;
}

void GUIInit(){
	pxWindowManagerCreate(bGUIonWindowManagerCreateHandler);
}

void vGUIHandlePeriph(eHW event, int32_t param1) {
  switch (event){
    case EV_HW_NFC:
      getHWStatus()->NFC++;
      break;
    case EV_HW_BTN_PRESS:
      if (param1 == 0) {
        getHWStatus()->b1++;
        break;
      }
      if (param1 == 1) {
        getHWStatus()->b2++;
        break;
      }
      break;
	case EV_HW_BTN_LONG:
  if (millis()  <5500) {
    // service mode on! 
    periph->sleepTimeout.stop();
    deviceState::getInstance()->serviceMode = true;
	deviceState::getInstance()->ssid = initWifi();
    deviceState::getInstance()->mqttServiceMode();
  } else {

  }
		WindowMain::getInstance()->open();
		break;
    case EV_HW_SYS_INT:
      getHWStatus()->SysINT++;
      break;
    case EV_HW_TOUCH_INT:
      getHWStatus()->TouchINT++;
      break;
    case EV_HW_VOLTAGE_CHANGE:
	  batteryLevelWarnings(param1);
	  break;
    case EV_HW_CHRG_STATE:
      switch (param1){
        case BAT_DISCHARGING:
          getHWStatus()->dischargeSW.start();
          getHWStatus()->chargeSW.stop();
          break;
        case BAT_CHARGING:
          getHWStatus()->chargeSW.start();
          getHWStatus()->dischargeSW.stop();
		  //WindowMessage::getInstance()->open("Нет пользователя", "Зарядка...");
          break;
        case BAT_CHARGED:
          getHWStatus()->chargeSW.stop();
          getHWStatus()->dischargeSW.stop(); 
		  WindowMessage::getInstance()->open("Аккумулятор заряжен", "100%");
          break;
        case BAT_UNKNOWN:
        case BAT_ERROR:
          break;
      } 
      break;
    case EV_HW_PROXIMITY:
	  periph->sleepTimeout.reset();
      getHWStatus()->ps = param1;
      getHWStatus()->SysINT++;

      break;

    case EV_WIFI:
      getHWStatus()->WiFiStatus = param1;
      switch (param1){
        case HW_CON:
            getHWStatus()->connON.start();
            getHWStatus()->connOFF.stop();
            getHWStatus()->connONi++;
          break;
        case HW_DISCON:
            getHWStatus()->connOFF.start();
            getHWStatus()->connON.stop();
            getHWStatus()->connOFFi++;
          break;
        case HW_GOTIP:
            getHWStatus()->connIPi++;
          break;
      }
      break;
  }
}
