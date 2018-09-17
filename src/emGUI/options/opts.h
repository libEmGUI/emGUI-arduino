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

// PictureStack
#ifndef PICTURE_STACK_MAX_SIZE
#define PICTURE_STACK_MAX_SIZE		15
#endif

// Dimensions
extern uint16_t EMGUI_LCD_WIDTH;
extern uint16_t EMGUI_LCD_HEIGHT;
extern uint16_t EMGUI_STATUS_BAR_HEIGHT; 
extern uint16_t EMGUI_MODAL_DLG_BTN_SPACING;

// String limits
extern uint16_t EMGUI_WINDOW_HEADER_LENGTH;
extern uint16_t EMGUI_LABEL_MAX_LENGTH;
extern uint16_t EMGUI_DEBUG;

// GUI COLORS

// RGB565 definition 
extern uint16_t EMGUI_COLOR_SEA_5; 		// Blue colors map from drakest to lightest
extern uint16_t EMGUI_COLOR_SEA_4;
extern uint16_t EMGUI_COLOR_SEA_3;
extern uint16_t EMGUI_COLOR_SEA_2;
extern uint16_t EMGUI_COLOR_SEA_1;

extern uint16_t EMGUI_COLOR_BROWN_5;	// Brown colors map from drakest to lightest
extern uint16_t EMGUI_COLOR_BROWN_4;
extern uint16_t EMGUI_COLOR_BROWN_3;
extern uint16_t EMGUI_COLOR_BROWN_2;
extern uint16_t EMGUI_COLOR_BROWN_1;

extern uint16_t EMGUI_COLOR_WHITE;
extern uint16_t EMGUI_COLOR_GRAY;
extern uint16_t EMGUI_COLOR_BLACK;

extern uint16_t EMGUI_COLOR_RED;
extern uint16_t EMGUI_COLOR_GREEN;
extern uint16_t EMGUI_COLOR_BLUE;


// STATUSBAR STORY
extern uint16_t EMGUI_COLOR_MENU_HEADER_TEXT; 
extern uint16_t EMGUI_COLOR_STATUS_BAR_BG;
extern uint16_t EMGUI_COLOR_MESSAGE_TEXT;
extern uint16_t EMGUI_MENU_BUTTON_LABEL_TEXT_COLOR;
extern uint16_t EMGUI_MENU_BUTTON_LABEL_BG_COLOR;

// PLOT Colors
extern uint16_t EMGUI_COLOR_PLOT_BACKGROUND;
extern uint16_t EMGUI_COLOR_PLOT_GRIDS;
extern uint16_t EMGUI_COLOR_PLOT_SCALE_MARKER;

#endif // !OPTS_H
