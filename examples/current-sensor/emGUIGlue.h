#ifndef _TFT_WRAPPER_H_
#define _TFT_WRAPPER_H_

#include <emGUI.h>



void TFTSleep();

void TFTWake();

void vGUIGlueInit();

xFont xGetDefaultFont();
void vGUIGlueSetCtx(void *);

//some gui settings
extern int EMGUI_COLOR_SW_BLUE;

#endif