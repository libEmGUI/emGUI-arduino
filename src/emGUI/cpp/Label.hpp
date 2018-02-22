#pragma once

#include "emGUI/emGUI.h"
#include "emGUI/cpp/Widget.hpp"
//#include <functional>

namespace emGUI {

	class Label : public Widget, public WidgetCaster<Label> {
	public:

		Label(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH, char const * cStr, xFont xFnt, uint16_t usMaxLength, xWidget *pxWidParent){
			xThis = pxLabelCreate(usX, usY, usW, usH, cStr,  xFnt,  usMaxLength, pxWidParent);
			xThis->pvUserData = this;
			xThis->pxDrawHandler = [](xWindow *pxW) -> bool {
				if (auto w = WidgetCaster<Label>::getObject(pxW))
					w->onDrawUpdate();

				return bLabelDraw(pxW);
			};
		}

		virtual bool onDrawUpdate() { return false; }

	};
}