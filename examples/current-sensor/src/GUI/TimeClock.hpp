#pragma once

#include "WindowPack.h"

#include "sys/time.h"
#define __STDC_LIB_EXT1__
#include "time.h"

using namespace emGUI;
class TimeClock : public Label {
public:
	typedef std::unique_ptr<TimeClock> uniquePtr;

	TimeClock(uint16_t usX, uint16_t usY, uint16_t usW, uint16_t usH,  xFont xFnt, uint16_t usMaxLength, xWidget *pxWidParent)
		: Label(usX, usY, usW, usH, "--:--", xFnt, usMaxLength, pxWidParent) {
		vLabelSetTextAlign(xThis, LABEL_ALIGN_CENTER);
	}

	bool onDrawUpdate() {
		auto now = time(NULL);

#if defined(_MSC_VER)
#ifdef __STDC_LIB_EXT1__
		struct tm tm;
		localtime_s(&tm, &now);
		struct tm *tv = &tm;
#endif
#else
		auto tv = localtime(&now);
#endif
		

		if (tv->tm_year < 118) { //time is not updated yet
			return true;
		}

		if (lastv != tv->tm_min) {
			lastv = tv->tm_min;
			
			iLabelPrintf(xThis, "%02d:%02d", tv->tm_hour, tv->tm_min);
		
			return true;
		}
		return false;
	}

	int lastv = -1;
};
