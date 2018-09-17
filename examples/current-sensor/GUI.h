#ifndef _GUI_H_
#define _GUI_H_

#include <Arduino.h>
#include "emGUIGlue.h"
#include "Peripherial.h"
#include <emGUI.h>
#include "src/common/Config.hpp"
#include "src/common/Stopwatch.h"
#include "src/common/UpdatableValue.hpp"
#include "src/DummyGUI/deviceState.hpp"

typedef enum {
	WINDOW_MENU,
	WINDOW_MALFUNCTION,
	WINDOW_TP_TEST,
	WINDOW_BATTERY,
	WINDOW_WIFI,
	WINDOW_IDLE,	//WindowIDLE
	WINDOW_POPUP,	//WindowPopUp
	WINDOW_SHORT_MESSAGE,
	WINDOW_PLOT
}eWindows;

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
void vGUIUpdateCurrentMonitor(short);



void vGUIHandlePeriph(eHW event, int32_t param1);

void GUIInit();
#endif //_GUI_H_
