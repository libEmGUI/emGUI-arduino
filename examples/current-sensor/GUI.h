#ifndef _GUI_H_
#define _GUI_H_

#include <Arduino.h>
#include "emGUIGlue.h"
#include "Peripherial.h"
#include "Stopwatch.h"
#include "UpdatableValue.hpp"
#include "Config.hpp"
#include <emGUI.h>
#include <Version.h>
#include "src/DummyGUI/deviceState.hpp"

typedef enum {
	WINDOW_MENU,
	WINDOW_MALFUNCTION,
	WINDOW_HW_TEST,
	WINDOW_TP_TEST,
	WINDOW_BATTERY,
	WINDOW_WIFI,
	WINDOW_IDLE,	//WindowIDLE
	WINDOW_POPUP,	//WindowPopUp
	WINDOW_TASK,	//WindowTask
	WINDOW_PAGE3,
	WINDOW_PAGE4,
	WINDOW_PAGE5,
	WINDOW_PAGE6,
	WINDOW_AUTH,
	WINDOW_SHORT_MESSAGE,
	WINDOW_PAGE9,
	WINDOW_PAGE10,
	WINDOW_PAGE11,
	WINDOW_PAGE12,
	WINDOW_PAGE13,
	WINDOW_PAGE14,
	WINDOW_PAGE15,
	WINDOW_PAGE16,
	WINDOW_PAGE17,


}eWindows;

typedef enum {
	EV_HW_NFC = Peripherial::EV_NFC, 
	EV_HW_BTN_PRESS = Peripherial::EV_BTN_PRESS,      //param is button number
    EV_HW_BTN_RELEASE = Peripherial::EV_BTN_RELEASE,        
    EV_HW_CHRG_STATE = Peripherial::EV_CHRG_STATE,   
    EV_HW_VOLTAGE_CHANGE = Peripherial::EV_VOLTAGE_CHANGE, 
    EV_HW_PROXIMITY = Peripherial::EV_PROXIMITY,
	EV_HW_BTN_LONG = Peripherial::EV_BTN_LONG,
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

typedef enum {
	HW_MQTT_CON,
	HW_MQTT_DISCON,
}eMQTTStatus;



class HWStatus {
public:
	HWStatus() {
		NFC = 0;
		b1 = 0;
		b2 = 0;
		SysINT = 0;
		TouchINT = 0;

		WiFiStatus = 0;
		connOFF.start();

	};

	void tick(){
		static int i = 0;
		chargeTime = (int)((int)chargeSW.get() / 1000);
		dischargeTime = (int)((int)dischargeSW.get() / 1000);	
		connONTime = (int)((int)connON.get() / 1000);
		connOFFTime = (int)((int)connOFF.get() / 1000);
	}

	UpdatableValue NFC;
	UpdatableValue ps;
	UpdatableValue b1;
	UpdatableValue b2;
	UpdatableValue SysINT;
	UpdatableValue TouchINT;

	Stopwatch chargeSW;
	Stopwatch dischargeSW;
	UpdatableValue chargeTime;
	UpdatableValue dischargeTime;

	Stopwatch connON;
	Stopwatch connOFF;
	UpdatableValue connONTime;
	UpdatableValue connOFFTime;
	UpdatableValue WiFiStatus;
	UpdatableValue connONi;
	UpdatableValue connOFFi;
	UpdatableValue connIPi;

};

HWStatus * getHWStatus();

void vGUIHandlePeriph(eHW event, int32_t param1);

void GUIInit();
#endif //_GUI_H_
