#ifndef _WINDOW_PACK_
#define _WINDOW_PACK_


#include "emGUI/emGUI.hpp"
#include "emGUI/emGUI.h"
//#include "emGUIGlue.h"
#include "../../GUI.h"


#define COLOR_BLUE EMGUI_COLOR_SEA_3
#define so 5
#define bo 14


xFont getBigFont();
xFont getSmallFont();
xFont getBoldFont();

#include "TimeClock.hpp"
#include "BatteryWidget.hpp"
#include "WiFiWidget.hpp"
#include "ButtonWithBg.hpp"
#include "TextFrame.hpp"
#include "MQTTWidget.hpp"



#include "WindowMessage.h"

#include "WindowAuth.h"
#include "WindowTask.h"
#include "WindowIDLE.h"
#include "WindowPopUp.h"




#endif // !_WINDOW_PACK_

