#ifndef _WINDOW_PAGE_2_
#define _WINDOW_PAGE_2_

#include "WindowPack.h"
using namespace emGUI;

class WindowTask : public DisposableWindow<WINDOW_TASK, WindowTask> {
public:
	void create();

	virtual bool onKeypress(uint16_t uEv) {
		return !deviceState::getInstance()->debug(); //suppress close by button when not debugging
	}

	virtual bool onOpen();

protected:
	TimeClock::uniquePtr clock;
	ButtonWithBG::uniquePtr btn_ok;
	ButtonWithBG::uniquePtr btn_cancel;
	TextFrame::uniquePtr frame;
	xLabel * title;
};

#endif // _WINDOW_PAGE_2_
