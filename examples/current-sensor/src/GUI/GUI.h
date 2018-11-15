#ifndef _GUI_H_
#define _GUI_H_

#include <Arduino.h>
#include <emGUI.h>
#include "../../emGUIGlue.h"
#include "../common/Stopwatch.h"
#include "../common/UpdatableValue.hpp"



typedef enum {
	EV_HW_SYS_INT = 100,
	EV_HW_TOUCH_INT = 101
} eHW;

xPlotData_t * pxGUIGetPlotData();

void vGUIHandlePeriph(eHW event, int32_t param1);

void GUIInit();
#endif //_GUI_H_
