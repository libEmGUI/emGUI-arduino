/**
 * @file   StatusBar.c
 * @author Roman Savrulin (romeo.deepmind@gmail.com)
 * @date   10.11.2012
 * @brief  Status bar widnget implementation
 *
 *
 */

#include <stdint.h>
#include <malloc.h>

#include "emGUI/Widgets/WindowManager.h"
#include "emGUI/Widgets/Button.h"
#include "emGUI/Widgets/Label.h"

#include "emGUI/Widgets/StatusBar.h"
#include "emGUI/Draw/Draw.h"
 /** @weakgroup prop-widget-statusbar
 *  @{
 */


typedef struct xStatusBarProps_struct {
	xButton             *xCloseButton;
	xLabel              *xWndHeader;
} xStatusBarProps;


 /**
  * @brief close button handler
  *
  * @returns true - event is handled
  */
static bool prvCloseClickHandler(xWidget* pxW) {
	(void)pxW;
	vWindowManagerCloseActiveWindow();
	return true;
}

xStatusBar* xStatusBarCreate(uint16_t usColor) {

	xStatusBar * pxW;
	xStatusBarProps * xP;
	uint16_t usX, usY, usW;

	xFont xFnt = pxDrawHDL()->xGetDefaultFont();

	pxW = (xStatusBar *) pxWidgetCreate(0, 0, usWindowManagerGetW(), EMGUI_STATUS_BAR_HEIGHT, pxWindowManagerGet(), true);

	if(!pxW){
		vWidgetDispose(pxW);
		return NULL;
	}

	xP = malloc(sizeof(xStatusBarProps));

	if (!xP) {
		free(pxW);
		return NULL;
	}

	memset(xP, 0, sizeof(xStatusBarProps));

	pxW->pvProp = xP;

	pxW->eType = WidgetStatusBar;
	vWidgetSetBgColor(pxW, usColor, false);

	return pxW;
}

void  vStatusBarSetWindowHeader(xStatusBar* pxW, const char * strH) {
	xStatusBarProps *xP;
	if (!(xP = (xStatusBarProps *)pxWidgetGetProps(pxW, WidgetStatusBar)))
		return;

	pcLabelSetText(xP->xWndHeader, strH);
}

/**
 *  @}
 */
