/*
Adapted from the Adafruit graphicstest sketch.

This sketch uses the GLCD font (font 1) only. Disable other fonts to make
the sketch fit in an UNO!

Make sure all the required fonts are loaded by editting the
User_Setup.h file in the TFT_ILI9341_DUE library folder.

If using an UNO or Mega (ATmega328 or ATmega2560 processor) then for best
performance use the F_AS_T option found in the User_Setup.h file in the
TFT_ILI9341_DUE library folder.

The library uses the hardware SPI pins only:
  For UNO, Nano, Micro Pro ATmega328 based processors
    MOSI = pin 11, SCK = pin 13
  For Mega:
    MOSI = pin 51, SCK = pin 52

The pins used for the TFT chip select (CS) and Data/command (DC) and Reset (RST)
signal lines to the TFT must also be defined in the library User_Setup.h file.

Sugested TFT connections for UNO and Atmega328 based boards
  sclk 13  // Don't change, this is the hardware SPI SCLK line
  mosi 11  // Don't change, this is the hardware SPI MOSI line
  cs   10  // Chip select for TFT display
  dc   9   // Data/command line
  rst  7   // Reset, you could connect this to the Arduino reset pin

Suggested TFT connections for the MEGA and ATmega2560 based boards
  sclk 52  // Don't change, this is the hardware SPI SCLK line
  mosi 51  // Don't change, this is the hardware SPI MOSI line
  cs   47  // TFT chip select line
  dc   48  // TFT data/command line
  rst  44  // you could alternatively connect this to the Arduino reset


for this demo , you need an arduino due with ili9341 TFT.
third-part library is ILI9341_due :https://github.com/marekburiak/ILI9341_due 
(git clone https://github.com/marekburiak/ILI9341_due.git )

#########################################################################
###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
######       TO SELECT THE FONTS AND PINS YOU USE, SEE ABOVE       ######
#########################################################################
*/


#include <emGUI.h>
#include "SPI.h"
#include "ILI9341_due.h"



// For the Adafruit shield, these are the default.
#define TFT_RST 8
#define TFT_DC 9
#define TFT_CS 10

// Use hardware SPI
ILI9341_due tft = ILI9341_due(TFT_CS, TFT_DC, TFT_RST);

typedef  uint8_t u8;

static xDraw_t LCD;
static xInterface * interface1;
static xLabel * mouseMonitor;

extern "C" {

  void vRectangle(uint16_t usX0, uint16_t usY0, uint16_t usX1, uint16_t usY1, uint16_t usColor, bool bFill){
    if (bFill) {
      tft.fillRect(usX0, usY0, usX1 - usX0, usY1 - usY0, usColor);
    } else {
      tft.drawRect(usX0, usY0, usX1 - usX0, usY1 - usY0, usColor);
    }
    
    
  }
  void vPutChar( uint16_t usX, uint16_t usY, char ASCI, xFont pubFont, uint16_t usColor, uint16_t usBackground, bool bFillBg){

    unsigned const char  *pubBuf = pubFont[(int)ASCI];
    unsigned char charWidth = *pubBuf; //each symbol in NEO fonts contains width info in first byte.
    unsigned char usHeight = *pubFont[0]; //each NEO font contains height info in first byte.
    pubBuf++; //go to first pattern of font
    uint16_t usXt, usYt;
    
    for (u8 column = 0; column < charWidth; column++) {
      usXt = usX + column;
      if( usXt >= ILI9341_TFTWIDTH)
        break;
      for (u8 row = 0; row < 8; row++){
          usYt = usY+row;
          if(*pubBuf & (1 << row))
            tft.drawPixel(usXt, usYt, usColor);
          else if(bFillBg)
            tft.drawPixel(usXt, usYt, usBackground);
          if( usYt >= ILI9341_TFTHEIGHT)
            break;
      };
      
      /* Hack for 16X NEO font */
      if(usHeight == 16){
        for (u8 row = 0; row < 8; row++){
          usYt = usY + row + 8;
          if(*(pubBuf + charWidth)& (1 << row))
            tft.drawPixel(usXt, usYt, usColor);
          else if(bFillBg)
            tft.drawPixel(usXt, usYt, usBackground);
          if( usYt >= ILI9341_TFTHEIGHT)
            break;
        };
      }
      pubBuf++;
    }
  }
  void vHLine(uint16_t usX0, uint16_t usY0, uint16_t usY1, uint16_t usColor){
    tft.drawFastVLine(usX0, usY0, usY1 - usY0, usColor);
  }
  void vVLine(uint16_t usX0, uint16_t usY0, uint16_t usX1, uint16_t usColor){
    tft.drawFastVLine(usX0, usY0, usX1 - usX0, usColor);
  }
  void bPicture(int16_t sX0, int16_t sY0, xPicture pusPicture){
    int16_t i, j;
    uint16_t x = 2;

    for (j = 0; j < pgm_read_byte(pusPicture + 1); j++) {
      for (i = 0; i < pgm_read_byte(pusPicture); i++ ) {
        tft.drawPixel(sX0 + j, sY0 + i, ((pgm_read_byte(pusPicture + x) << 8)| pgm_read_byte(pusPicture + x + 1) ));
        x++;
      }
    }    
  }
  uint16_t usGetPictureW(xPicture pusPic) {
    return pgm_read_byte(pusPic + 1);
  }

  uint16_t usGetPictureH(xPicture pusPic) {
    return pgm_read_byte(pusPic + 0);
  }

};

unsigned long total = 0;
unsigned long tn = 0;

xWidget frame1;
xWindow * window;
xLabel label1;
eWindow wind1 = WINDOW_MENU;

void closeWindow() {
	;
}
bool bGUIOnWindowCloseHandlerMain(xWidget *) {
	auto dial = iModalDialogOpen(MODAL_AUTO, "ny", "Close?", "");
	vModalDialogSetHandler(dial, 'y', closeWindow);
	return true;
}
void doMagic() {
  char outString[25];
  static int i = 0;
  i++;
  sprintf(outString, "Magic count: %d times", i);
  pcLabelSetText(mouseMonitor, outString);
}
bool btnMagicHDLR(xWidget *) {
  auto dlg = iModalDialogOpen(MODAL_AUTO, "ny", "Magic Button", "Magic will happen. Are you sure?");
  vModalDialogSetHandler(dlg, 'y', &doMagic);
  return true;
}

bool btnAboutHDLR(xWidget *) {
  vInterfaceOpenWindow(WINDOW_ABOUT);
  return true;
}

bool btnLabelHDLR(xWidget *) {
  //vInterfaceOpenWindow(WINDOW_NOTES);
  return true;
}

bool btnFolderHDLR(xWidget *) {
  //vInterfaceOpenWindow(WINDOW_ARCHIVE);
  return true;
}


bool myHandler(xWidget *){
  auto window = pxWindowCreate(WINDOW_MENU);
  vWindowSetHeader(window, "Main menu");
  mouseMonitor = pxLabelCreate(1, 200, 238, 0, "Magic count: 0", FONT_ASCII_8_X, 500, window);
  uint8_t offset = 15;

  uint8_t row1 = offset;
  uint8_t row2 = row1 + 60 + offset;
  uint8_t row3 = row2 + 60 + offset;
  uint8_t column1 = offset;
  uint8_t column2 = LCD_SizeX / 2 - 30;
  uint8_t column3 = LCD_SizeX - offset - 60;
  auto menuBut		= pxMenuButtonCreate(column1, row1, magic, "Do magic", &btnMagicHDLR, window);
  auto menuButAbout	= pxMenuButtonCreate(column2, row1, help, "Info", &btnAboutHDLR, window);
  auto menuButLabel	= pxMenuButtonCreate(column3, row1, process, "Test Label", &btnLabelHDLR, window);
  auto menuButFolder  = pxMenuButtonCreate(column1, row2, open_folder, "Many windows", &btnFolderHDLR, window);

  auto window2_about = pxWindowCreate(WINDOW_ABOUT);
  vWindowSetHeader(window2_about, "About");

  auto window_show_label = pxWindowCreate(WINDOW_NOTES);
  vWindowSetHeader(window_show_label, "Big label");

  auto window_show_folder = pxWindowCreate(WINDOW_ARCHIVE);
  vWindowSetHeader(window_show_folder, "Labels");
  /*auto menuButAbout2 = pxMenuButtonCreate(column1, row1, PIC_LABEL_BG, "Info", &btnAboutHDLR, window_show_folder);
  auto menuButLabel2 = pxMenuButtonCreate(column2, row1, PIC_LABEL_BG, "Test Label", &btnLabelHDLR, window_show_folder);*/

  auto big_label = pxLabelCreate(1, 1, 238, 238, "Sample text: hypothetical rosters of players \
considered the best in the nation at their respective positions\
The National Collegiate Athletic Association, a college sports \
governing body, uses officially recognized All-America selectors \
to determine the consensus selections. These are based on a point \
system in which a player is awarded three points for every selector \
that names him to the first team, two points for the second team, \
and one point for the third team. The individual who receives the \
most points at his position is called a consensus All-American.[4] \
Over time, the sources used to determine the consensus selections \
have changed, and since 2002, the NCAA has used five selectors, \
the Associated Press (AP), American Football Coaches Association \
(AFCA), Football Writers Association of America (FWAA), The Sporting \
News (TSN), and the Walter Camp Football Foundation (WCFF),   \
to determine consensus All-Americans.[5]", FONT_ASCII_8_X, 1010, window_show_label);

  auto labelAbout = pxLabelCreate(1, 1, 238, 60, "This is Demo for emGUI. 2017", FONT_ASCII_8_X, 200, window2_about);

  vWindowSetOnCloseRequestHandler(window, &bGUIOnWindowCloseHandlerMain);
  auto dialog1 = pxModalDialogWindowCreate();
  vInterfaceOpenWindow(WINDOW_MENU);
  return true;
  
  return true;

}



void setup() {
  vDrawHandlerInit(&LCD);
	LCD.vRectangle = &vRectangle;
	LCD.vPutChar = &vPutChar;
	LCD.bPicture = &bPicture;
	LCD.vVLine = &vVLine;
	LCD.vHLine = &vHLine;
  LCD.usGetPictureH = &usGetPictureH;
  LCD.usGetPictureW = &usGetPictureW;
	vDrawSetHandler(&LCD);
  yield();Serial.begin(9600);
  while (!Serial);
  yield();Serial.println("emGUI testing");
  // Initial setup
  tft.begin();
  //tft.setRotation(iliRotation270);	// landscape
  tft.fillScreen(ILI9341_BLACK);
  
  interface1 = pxInterfaceCreate(&myHandler);
  
  vInterfaceOpenWindow(WINDOW_MENU);
}



uint16_t color;

void loop(void) {
  vInterfaceDraw();
  delay(3000);
  btnAboutHDLR(NULL);
  vInterfaceDraw();
  delay(3000);
  vInterfaceCloseActiveWindow();
  vInterfaceDraw();

}