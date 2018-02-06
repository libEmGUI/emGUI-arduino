#include <emGUI.h>
#include "emGUIGlue.h"
#include "Gui.h"

 
/*
Adapted from the Adafruit graphicstest sketch.

This sketch uses the GLCD font (font 1) only. Disable other fonts to make
the sketch fit in an UNO!

Make sure all the required fonts are loaded by editting the
User_Setup.h file in the TFT_ILI9341_ESP library folder.

If using an UNO or Mega (ATmega328 or ATmega2560 processor) then for best
performance use the F_AS_T option found in the User_Setup.h file in the
TFT_ILI9341_ESP library folder.

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

#########################################################################
###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
######       TO SELECT THE FONTS AND PINS YOU USE, SEE ABOVE       ######
#########################################################################
*/

void setup() {
  vGUIGlueInit();
  GUIInit();
}

void loop(void) {
  vWindowManagerDraw();
}
 
