#pragma once

#include "WindowPack.h"

using namespace emGUI;
class MQTTWidget : public PictureStack {
public:
	typedef std::unique_ptr<MQTTWidget> uniquePtr;

	MQTTWidget(uint16_t usX, uint16_t usY, xWidget *pxWidParent)
		: PictureStack(usX, usY, "/mqtt/0.bmp", pxWidParent) {
		bPictureStackAddItem(xThis, "/mqtt/1.bmp");

	}

	bool onDrawUpdate() {
        bool disconnected = !deviceState::getInstance()->mqttReady();
        if (disconnected) bPictureStackSelect(xThis, 0);
        else bPictureStackSelect(xThis, 1);
		
        return false;
	}
    int getWidth(){
		return usWidgetGetW(xThis);
	}
};
