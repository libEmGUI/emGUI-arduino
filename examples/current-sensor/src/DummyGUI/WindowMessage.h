#ifndef _WINDOW_PAGE_8_
#define _WINDOW_PAGE_8_

#include "WindowPack.h"

using namespace emGUI;
class WindowMessage : public DisposableWindow<WINDOW_SHORT_MESSAGE, WindowMessage> {
public:
	void create();

	xLabel *  title;
	xLabel * header;

	void open(char* tit, char * msg) {
		pcLabelSetText(title, tit);
		pcLabelSetText(header, msg);
		vWindowManagerOpenWindow(thisWndID);
	}
};



#endif // !_WINDOW_PAGE_8_

