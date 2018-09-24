#ifndef _GUI_H_
#define _GUI_H_

#include <Arduino.h>
#include <emGUI.h>
#include "../../emGUIGlue.h"
#include "../../Peripherial.h"
#include "../common/Config.hpp"
#include "../common/Stopwatch.h"
#include "../common/UpdatableValue.hpp"
#include "deviceState.hpp"



typedef enum {
	EV_HW_SYS_INT = 100,
	EV_HW_TOUCH_INT = 101,
	EV_WIFI = 102,
	EV_MQTT,

}eHW;

typedef enum {
	HW_CON = 1,
	HW_DISCON = -1,
	HW_GOTIP = 2
}eWiFiStatus;

xPlotData_t * pxGUIGetPlotData();

extern bool skipDelay;

void vGUIHandlePeriph(eHW event, int32_t param1);

void GUIInit();
#endif //_GUI_H_
