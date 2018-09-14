#pragma once

#include "WindowPack.h"
#include "deviceState.hpp"

using namespace emGUI;
class WiFiWidget : public PictureStack {
public:
	typedef std::unique_ptr<WiFiWidget> uniquePtr;

	WiFiWidget(uint16_t usX, uint16_t usY, xWidget *pxWidParent)
		: PictureStack(usX, usY, "/wf/0.bmp", pxWidParent) {
		addPicture("/wf/1.bmp");
		addPicture("/wf/2.bmp");

	}

	void selectItem() {
		int number = 0;

		switch (deviceState::getInstance()->wifiStatus){
			case deviceState::WIFI_DISC:
				number = 0;
				break;
			case deviceState::WIFI_CONN:
				number = 1;
				break;
			case deviceState::WIFI_GOTIP:
				number = 2;
				break;
		}


		bPictureStackSelect(xThis, (char)number);
	}
	void addPicture(xPicture xPic) {
		bPictureStackAddItem(xThis, xPic);
	}

	bool onDrawUpdate() {
		selectItem();
		return false;
	}

	int getWidth(){
		return usWidgetGetW(xThis);
	}
};
