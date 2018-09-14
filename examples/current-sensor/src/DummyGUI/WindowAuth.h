#ifndef _WINDOW_PAGE_7_
#define _WINDOW_PAGE_7_

#include "WindowPack.h"
#include "deviceState.hpp"

using namespace emGUI;
class WindowAuth : public DisposableWindow<WINDOW_AUTH, WindowAuth> {
public:
	void create();

	virtual bool onKeypress(uint16_t uEv) {
		return !deviceState::getInstance()->debug(); //suppress close by button when not debugging
	}
};



#endif // !_WINDOW_PAGE_7_
