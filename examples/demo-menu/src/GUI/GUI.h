#ifndef _GUI_H_
#define _GUI_H_

#include <Arduino.h>
#include "../../emGUIGlue.h"

typedef enum {
	WINDOW_MENU,
	WINDOW_HW_TEST,
	WINDOW_TP_TEST,
}eWindows;


void GUIInit();
#endif //_GUI_H_
