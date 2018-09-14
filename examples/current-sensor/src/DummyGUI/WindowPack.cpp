#include "WindowPack.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include "./../UTF8-fonts/customfonts.h"

const GFXfont * bigFont[] = { &Roboto_Regular24pt7b , &Roboto_Regular24pt8bRUS, NULL };
const GFXfont * smallFont[] = { &Roboto_Regular9pt7b , &Roboto_Regular9pt8bRUS, NULL };
const GFXfont * boldFont[] = { &Roboto_Bold9pt7b , &Roboto_Bold9pt8bRUS, NULL };


xFont getBigFont(){
	return bigFont;
}
xFont getSmallFont(){
	return smallFont;
}
xFont getBoldFont(){
	return boldFont;
}

