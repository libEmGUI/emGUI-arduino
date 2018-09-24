#ifndef _WINDOW_PAGE_1_
#define _WINDOW_PAGE_1_

#include "WindowPack.h"

using namespace emGUI;


class WindowPopUp : public DisposableWindow<WINDOW_POPUP, WindowPopUp> {
public:
	Button::uniquePtr frame;
	void create();
	TimeClock::uniquePtr clock;

};

#endif // _WINDOW_PAGE_0_
