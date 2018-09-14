#ifndef _WINDOW_PAGE_0_
#define _WINDOW_PAGE_0_

#include "WindowPack.h"
using namespace emGUI;


class WindowIDLE : public DisposableWindow<WINDOW_IDLE, WindowIDLE> {
public:
	void create() {
		auto x = 0;
		auto w = EMGUI_LCD_WIDTH;
		auto y = 40;
		auto h = 0;
		
		clock = std::make_unique<TimeClock>(x, y, w, h, getBigFont(), 6, xThis);
	}
	TimeClock::uniquePtr clock;

	virtual bool onCloseRequest() {
		return false;
	}

	bool onDrawUpdate(){
		if (deviceState::getInstance()->serviceMode){
      		bWidgetSetBgPicture(xThis, "/service.bmp");
    	}
		return false;
	}

};

#endif // _WINDOW_PAGE_0_
