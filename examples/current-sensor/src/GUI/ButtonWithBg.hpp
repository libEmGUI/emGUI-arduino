#pragma once

#include "WindowPack.h"

using namespace emGUI;
class ButtonWithBG : public Button, public WidgetCaster<ButtonWithBG> {
public:
	typedef std::unique_ptr<ButtonWithBG> uniquePtr;
	
	ButtonWithBG(uint16_t usX, uint16_t usY, const char *text, Widget *pxWidParent)
		: Button(usX, usY, pxDrawHDL()->usGetPictureW("/btn.bmp"), pxDrawHDL()->usGetPictureH("/btn.bmp"), text, pxWidParent) {

		bWidgetSetBgPicture(xThis, "/btn.bmp");
		vButtonSetText(xThis, text);
		vButtonSetTextColor(xThis, EMGUI_COLOR_WHITE);
	}
};


