#ifndef _TFT_WRAPPER_H_
#define _TFT_WRAPPER_H_

#include <emGUI.h>



void TFTSleep();

void TFTWake();

void vGUIGlueInit();

xFont xGetDefaultFont();
void vGUIGlueSetCtx(void *);


#endif