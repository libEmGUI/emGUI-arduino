#pragma once
#include "emGUI/emGUI.h"
#include "emGUI/cpp/Widget.hpp"

namespace emGUI {

	class PictureStack : public Widget, public WidgetCaster<PictureStack> {
	public:

		PictureStack(uint16_t usX, uint16_t usY, xPicture xPic, xWidget *pxWidParent) {
			xThis = pxPictureStackCreate(usX, usY, xPic, pxWidParent);
			xThis->pvUserData = this;
			xThis->pxDrawHandler = [](xWindow *pxW) -> bool {
				if (auto w = WidgetCaster<PictureStack>::getObject(pxW))
					w->onDrawUpdate();

				return bPictureStackDraw(pxW);
			};
		}

		virtual bool onDrawUpdate() { return false; }

	};
}
