#pragma once
#include "WindowPack.h"


using namespace emGUI;
class TextFrame : public Button {
public:
	typedef std::unique_ptr<TextFrame> uniquePtr;
	int cornerSize;
	int scrollSize;
	xLabel * text;
	TextFrame(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, char const * cStr, xFont xFnt, uint16_t usMaxLength, Widget * pxWidParent)
		: Button(usX, usY, usW, usH, "", pxWidParent) {

		cornerSize = 6;
		scrollSize = 2;
		auto corner1 = pxWidgetCreate(0, 0, cornerSize, cornerSize, xThis, true);
		auto corner2 = pxWidgetCreate(usWidgetGetW(xThis) - cornerSize, 0, cornerSize, cornerSize, xThis, true);
		auto corner3 = pxWidgetCreate(usWidgetGetW(xThis) - cornerSize, usWidgetGetH(xThis) - cornerSize, cornerSize, cornerSize, xThis, true);
		auto corner4 = pxWidgetCreate(0, usWidgetGetH(xThis) - cornerSize, cornerSize, cornerSize, xThis, true);

		bWidgetSetBgPicture(corner1, "/frame/1.bmp");
		bWidgetSetBgPicture(corner2, "/frame/2.bmp");
		bWidgetSetBgPicture(corner3, "/frame/3.bmp");
		bWidgetSetBgPicture(corner4, "/frame/4.bmp");

		text = pxLabelCreate(usX + cornerSize, usY + cornerSize, usW - 2*cornerSize - scrollSize, usH - 2*cornerSize, cStr, xFnt, usMaxLength, pxWidParent->get());

	
		xThis->pxDrawHandler = [](xWindow *pxW) -> bool {
			if (auto w = WidgetCaster<TextFrame>::getObject(pxW)) {
				w->onDrawUpdate();
			}
			return false;//vWidgetDraw(pxW);
		};
		onClick = [this](Button *b) {
			static bool direction = true;
			if (direction) {
				direction =	bLabelDrawNextPage(text);
			}
			else {
				direction = ~bLabelDrawPrevPage(text);
			}
		};
	}


	bool onDrawUpdate() {
		pxDrawHDL()->vVLine(xThis->usX0, xThis->usY0 + cornerSize, xThis->usY1 - cornerSize, EMGUI_COLOR_BLACK);
		pxDrawHDL()->vHLine(xThis->usX0 + cornerSize, xThis->usY0, xThis->usX1 - cornerSize, EMGUI_COLOR_BLACK);

		pxDrawHDL()->vVLine(xThis->usX1, xThis->usY0 + cornerSize, xThis->usY1 - cornerSize, EMGUI_COLOR_BLACK);
		pxDrawHDL()->vHLine(xThis->usX0 + cornerSize, xThis->usY1, xThis->usX1 - cornerSize, EMGUI_COLOR_BLACK);

		//TODO: remove this debug tool?
		if (xThis->bPressed) {
			pxDrawHDL()->vVLine(xThis->usX0, xThis->usY0 + cornerSize, xThis->usY1 - cornerSize, EMGUI_COLOR_GRAY);
			pxDrawHDL()->vHLine(xThis->usX0 + cornerSize, xThis->usY0, xThis->usX1 - cornerSize, EMGUI_COLOR_GRAY);

			pxDrawHDL()->vVLine(xThis->usX1, xThis->usY0 + cornerSize, xThis->usY1 - cornerSize, EMGUI_COLOR_GRAY);
			pxDrawHDL()->vHLine(xThis->usX0 + cornerSize, xThis->usY1, xThis->usX1 - cornerSize, EMGUI_COLOR_GRAY);

		}

		pxDrawHDL()->vRectangle(xThis->usX1 - scrollSize - cornerSize, xThis->usY0 + cornerSize, xThis->usX1 - cornerSize, xThis->usY1 - cornerSize, EMGUI_COLOR_SW_BLUE, true);
		return false;
	}

	void setText(const char * str) {
		pcLabelSetText(text, str);
	}
};
