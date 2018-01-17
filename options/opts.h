#ifndef EMGUI_DEFAULT_OPTS_H
#define EMGUI_DEFAULT_OPTS_H

#include "emGUI_port_opts.h"

// DEFAULT image and font types

#ifndef EMGUI_XPICTURE_TYPE
#error "Please define EMGUI_XPICTURE_TYPE"
#endif

#ifndef EMGUI_XFONT_TYPE
#error "Please define EMGUI_XFONT_TYPE"
#endif

typedef EMGUI_XPICTURE_TYPE			xPicture;
typedef EMGUI_XFONT_TYPE			xFont; //TODO: Add GFXFont segmented struct to allow UTF-8 fonts support!

// IDs

#ifndef EMGUI_MODAL_AUTO
#define EMGUI_MODAL_AUTO			1000
#endif

#ifndef EMGUI_MODAL_WINDOW_ID
#define EMGUI_MODAL_WINDOW_ID		1000
#endif

// Dimensions

#ifndef EMGUI_MODAL_DLG_BTN_SPACING
#define EMGUI_MODAL_DLG_BTN_SPACING	100
#endif

#ifndef EMGUI_LCD_WIDTH
#define EMGUI_LCD_WIDTH				240
#endif

#ifndef EMGUI_LCD_HEIGHT
#define EMGUI_LCD_HEIGHT			240
#endif

#ifndef EMGUI_STATUS_BAR_HEIGHT
#define EMGUI_STATUS_BAR_HEIGHT		26
#endif

// String limits

#ifndef EMGUI_WINDOW_HEADER_LENGTH
#define EMGUI_WINDOW_HEADER_LENGTH	25
#endif

#ifndef EMGUI_LABEL_MAX_LENGTH
#define EMGUI_LABEL_MAX_LENGTH		1000
#endif

#ifndef EMGUI_DEBUG
#define EMGUI_DEBUG					0
#endif

// GUI COLORS

// RGB565 definition 
#define EMGUI_COLOR_SEA_5 0x02AC	// Sea colors map from drakest to lightest
#define EMGUI_COLOR_SEA_4 0x1B2E
#define EMGUI_COLOR_SEA_3 0x0413
#define EMGUI_COLOR_SEA_2 0x3599
#define EMGUI_COLOR_SEA_1 0x5DD9

#define EMGUI_COLOR_BROWN_5 0xA280	// Earth colors map from drakest to lightest
#define EMGUI_COLOR_BROWN_4 0xBBA5
#define EMGUI_COLOR_BROWN_3 0xFBC0
#define EMGUI_COLOR_BROWN_2 0xFCC7
#define EMGUI_COLOR_BROWN_1 0xFDAD

#define EMGUI_COLOR_WHITE	0xFFFF
#define EMGUI_COLOR_BLACK	0

#define EMGUI_COLOR_RED     0xF800
#define EMGUI_COLOR_GREEN   0x7E0
#define EMGUI_COLOR_BLUE    0x1F


// STATUSBAR STORY
#ifndef EMGUI_COLOR_MENU_HEADER_TEXT
#define EMGUI_COLOR_MENU_HEADER_TEXT EMGUI_COLOR_WHITE //WHITE
#endif

#ifndef EMGUI_COLOR_STATUS_BAR_BG
#define EMGUI_COLOR_STATUS_BAR_BG EMGUI_COLOR_SEA_5 //light-blue
#endif

#ifndef EMGUI_COLOR_MESSAGE_TEXT
#define EMGUI_COLOR_MESSAGE_TEXT EMGUI_COLOR_BLACK
#endif

#ifndef EMGUI_MENU_BUTTON_LABEL_TEXT_COLOR
#define EMGUI_MENU_BUTTON_LABEL_TEXT_COLOR EMGUI_COLOR_BLACK
#endif

#ifndef EMGUI_MENU_BUTTON_LABEL_BG_COLOR
#define EMGUI_MENU_BUTTON_LABEL_BG_COLOR EMGUI_COLOR_WHITE
#endif

// PLOT Colors

#ifndef EMGUI_COLOR_PLOT_BACKGROUND
#define EMGUI_COLOR_PLOT_BACKGROUND EMGUI_COLOR_WHITE
#endif

#ifndef EMGUI_COLOR_PLOT_GRIDS
#define EMGUI_COLOR_PLOT_GRIDS EMGUI_COLOR_SEA_1
#endif

#ifndef EMGUI_COLOR_PLOT_SCALE_MARKER
#define EMGUI_COLOR_PLOT_SCALE_MARKER    EMGUI_COLOR_BROWN_5
#endif


#endif // !OPTS_H
