#pragma once

using namespace emGUI;
class WindowHeader : public Label {
public:
	typedef std::unique_ptr<WindowHeader> uniquePtr;

	WindowHeader(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, xFont xFnt, uint16_t usMaxLength, xWidget *pxWidParent)
		: Label(usX, usY, usW, usH, "", xFnt, usMaxLength, pxWidParent) {
		vLabelSetTextAlign(xThis, LABEL_ALIGN_CENTER);
		vWidgetSetBgColor(xThis, EMGUI_COLOR_STATUS_BAR_BG, false);
		vLabelSetTextColor(xThis, EMGUI_COLOR_MENU_HEADER_TEXT);
		vLabelSetTextAlign(xThis, LABEL_ALIGN_CENTER);
	}

	bool onDrawUpdate() {
		//TODO: what if pass NULL? 
		pcLabelSetText(xThis, vWindowGetHeader(pxWindowManagerGetActiveWindow()));
		
		return false;
	}

};
