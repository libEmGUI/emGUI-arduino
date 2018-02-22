#pragma once

#include "emGUI/emGUI.h"
#include "emGUI/cpp/Widget.hpp"
#include <functional>

namespace emGUI {

	class ButtonBase : public Widget {
	public:
		virtual ~ButtonBase() {
			if (xThis) {
				vButtonSetOnClickHandler(xThis, NULL);
			}
		};

		ButtonBase(uint16_t X, uint16_t Y, uint16_t W, uint16_t H, const char *text, Widget * parent) {
			xThis = pxButtonCreateFromText(X, Y, W, H, text, parent->get());
			xThis->pvUserData = this;
		}
	};

	class Button : public ButtonBase, public WidgetCaster<Button> {
	public:

		Button(uint16_t X, uint16_t Y, uint16_t W, uint16_t H, const char *text, Widget * parent):
		ButtonBase(X, Y, W, H, text, parent) {
			xThis->pvUserData = this;

			vButtonSetOnClickHandler(xThis, [](xWidget *pxW) -> bool {
				if (auto o = WidgetCaster<Button>::getObject(pxW)) {
					if(o->onClick)
						o->onClick(o);
				}
				return true;
			});
		}

		std::function<void(Button *b)> onClick;
	};

	class ButtonToggle : public ButtonBase, public WidgetCaster<ButtonToggle>{
	public:

		ButtonToggle(uint16_t X, uint16_t Y, uint16_t W, uint16_t H, const char *text, Widget * parent):
			ButtonBase(X, Y, W, H, text, parent), _state(false) {
			xThis->pvUserData = this;
			
			vButtonSetOnClickHandler(xThis, [](xWidget *pxW) -> bool {
				if (auto o = WidgetCaster<ButtonToggle>::getObject(pxW)) {
					o->click();
				}
				return true;
			});
			toggleDecorator();
		}

		void state(bool state) {
			_state = state;
			toggleDecorator();
			if (onToggle)
				onToggle(this, _state);
		}

		bool state() {
			return _state;
		}

		void click() {
			if (onClick)
				onClick(this);
			
			if (onToggle) {
				if(onToggle(this, !_state))  // if there's a handler, toggle is done when it return true 
					_state = !_state;
			} else 
				_state = !_state;
			toggleDecorator();
		};

		virtual void toggleDecorator() {
			if (_state)
				vWidgetSetBgColor(get(), EMGUI_COLOR_RED, false);
			else
				vWidgetSetBgColor(get(), EMGUI_COLOR_GREEN, false);
		}

		std::function<bool(ButtonToggle *b, bool)> onToggle;
		std::function<void(ButtonToggle *b)> onClick;
	protected:
		bool _state;
	};
}