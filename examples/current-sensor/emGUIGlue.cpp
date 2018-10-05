#include "emGUIGlue.h"

#include "SPI.h"
#include "src/TFT/TFT_ILI9341_ESP.h"

#include "FS.h"

#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "src/UTF8-fonts/MyriadPro_Regular9pt8b.h"

#include <cstddef>

#include <emGUI_port_opts.h>




template<typename s, int t> struct check_size {
  static_assert(sizeof(s) == t, "wrong size");
};

/***********************  BITMAP READ      *************************/
typedef struct {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

check_size<BITMAPFILEHEADER, 14> CHECK_BITMAPFILEHEADER;

typedef struct {
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  uint32_t biXPelsPerMeter;
  uint32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;

check_size<BITMAPINFOHEADER, 40> CHECK_BITMAPINFOHEADER;

#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))

//SPI9Class SPI9(TFT_DC);                   // construct global SPI9 instance here, where the dcPin is known
static TFT_ILI9341_ESP *tft = NULL;
static xDraw_t LCD;

class BMPFile{
public:
  BMPFile(): _w(0), _h(0), _offs(0){

  }

  operator bool() const{
    return (bool)_f;
  }

  void seekData(){
    _f.seek(_offs, SeekSet);
  }

  bool open(const char * fn){
    if (_f && (strcmp(fn, _f.name()) == 0)){
      //Serial.print(fn);
      //Serial.println(" Cache hit");
      return true;
    }

    //Serial.print(fn);
    //Serial.println(" open");

    _f.close();
    _f = SPIFFS.open(fn, "r");

    if (!_f) {
      Serial.print(fn);
      Serial.println(" file open failed");
      return false;
    }

    BITMAPFILEHEADER picHeader;
    BITMAPINFOHEADER picInfo;

    _f.readBytes((char*)&picHeader, sizeof(picHeader));

    if (picHeader.bfType != 0x4D42)
      return false;

    _f.readBytes((char*)&picInfo, sizeof(picInfo));
    _w = picInfo.biWidth;
    _h = picInfo.biHeight;
    _offs = picHeader.bfOffBits;

    return true;
  }

  size_t readBytes(char *buffer, size_t length) {
      return _f.readBytes(buffer, length);
  } 

  uint16_t width() {
    return _w;
  }

  uint16_t height() {
    return _h;
  }

protected:
  File _f;
  uint16_t _w;
  uint16_t _h;
  uint16_t _offs;
};

static BMPFile *bmFile = NULL;

typedef enum {
  UTF_WAIT_FIRST = 0,
  UTF_WAIT_SECOND = 1
}eUTFstate;

typedef struct {
  eUTFstate state;
  uint16_t buffer;
} tUTFstateMachine;

bool vGUICheckUTF(uint8_t data, tUTFstateMachine * t_status) {      //returns true if we got whole char
  static const uint8_t firstMarker= 0b11000000;
  static const uint8_t firstMask  = 0b11100000;

  static const uint8_t secondMarker  = 0b10000000;
  static const uint8_t secondMask    = 0b11000000;
  
  tUTFstateMachine * status = t_status;

  switch (t_status->state) {
  case UTF_WAIT_FIRST:
    if ((data & firstMask) == firstMarker) {
      t_status->state = UTF_WAIT_SECOND;// we catched first symbol
      t_status->buffer = 0;
      t_status->buffer = (data & ~firstMask) << 6;
      return false;
    }
    
    t_status->buffer = data;
    return true;

  case UTF_WAIT_SECOND:
    t_status->state = UTF_WAIT_FIRST;
    if ((data & secondMask) == secondMarker) {
      t_status->buffer |= data & ~secondMask;
      return true;
    }
    if (data < 0x80) { //can be valid ASCII character
      t_status->buffer = data;
      return true;
    }
    t_status->buffer = 0;
    return false;
  }
}

static uint16_t usFontGetH(xFont pubFont) {
  const GFXfont *gfxFont = *pubFont;
  uint8_t  height = pgm_read_byte (&gfxFont->yAdvance);
  return (uint16_t) height;
}

static xModalDialogPictureSet prvGetPicSet(char cType) {
  xModalDialogPictureSet xPicSet;
  switch (cType) {
  case 'n':
    xPicSet.strLabel = "No";
    xPicSet.xPicMain = "/no.bmp";
    break;
  case 'c':
    xPicSet.strLabel = "Cancel";
    xPicSet.xPicMain = "/no.bmp";
    break;
  case 'o':
    xPicSet.strLabel = "OK";
    xPicSet.xPicMain = "/ok59.bmp"; //for debug purposses use 59 pixels ok picture
    break;
  default:
    xPicSet.strLabel = "OK";
    xPicSet.xPicMain = "/ok59.bmp";
    break;
  }

  return xPicSet;
}

static const GFXfont * prvGetFontSegment(uint16_t ch, xFont f) {
#if EMGUI_USE_UTF8_FONTS
  while (*f) {
    uint16_t first = (uint16_t)pgm_read_word(&(*f)->first);
    uint16_t last = (uint16_t)pgm_read_word(&(*f)->last);

    if (ch >= first && ch <= last)
      return *f;
    f++;
  }
#else
  return f;
#endif // EMGUI_USE_UTF8_FONTS
  return NULL;
}

static uint16_t ucFontGetCharW(char cChar, xFont pubFont) {
  static tUTFstateMachine machine = { UTF_WAIT_FIRST,0 };
  uint16_t utfChar;
  if (!vGUICheckUTF(cChar, &machine)) {
    return 0;
  }
  else {
    utfChar = machine.buffer;
  }
  const GFXfont *gfxFont = prvGetFontSegment(utfChar, pubFont);
  if (!gfxFont)
    return 0;

  utfChar -= (uint16_t)pgm_read_word(&gfxFont->first);
  GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[utfChar]);

  uint8_t  w  = pgm_read_byte(&glyph->xAdvance);
  return (uint16_t) w;
}

static uint16_t usFontGetStrW(char const * pcStr, xFont pubFont) {
  uint16_t usWidth = 0;

  while (*pcStr) {
    usWidth += ucFontGetCharW(*pcStr, pubFont);
    pcStr++;
  }
  return usWidth;
}

static uint16_t usFontGetStrH(const char * pcStr, xFont pubFont) {
  //TODO: implement multistring height.
  return usFontGetH(pubFont);
}

static void vRectangle(uint16_t usX0, uint16_t usY0, uint16_t usX1, uint16_t usY1, uint16_t usColor, bool bFill) {
  if (!tft)
    return;
  if (bFill) {

    tft->fillRect(usX0, usY0, usX1 - usX0 + 1, usY1 - usY0 + 1, usColor);
  } else {
    tft->drawRect(usX0, usY0, usX1 - usX0 + 1, usY1 - usY0 + 1, usColor);
  }
}

static void vPutChar( uint16_t usX, uint16_t usY, char cChar, xFont pubFont, uint16_t usColor, uint16_t usBackground, bool bFillBg) {
  static tUTFstateMachine machine = { UTF_WAIT_FIRST,0 };
  uint16_t utfChar;
  if (!vGUICheckUTF(cChar, &machine)) {
    return ;
  }
  else {
    utfChar = machine.buffer;
  }
  const GFXfont *gfxFont = prvGetFontSegment(utfChar, pubFont);
  if (!gfxFont)
    return ;


  utfChar -= (uint16_t)pgm_read_word(&gfxFont->first);
  GFXglyph *glyph  = &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[utfChar]);
  uint8_t  *bitmap = (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);

  uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
  uint8_t  w  = pgm_read_byte(&glyph->width),
           h  = pgm_read_byte(&glyph->height);
  int8_t   xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
  uint8_t  xx, yy, bits = 0, bit = 0;

  auto x = usX;
  auto y = usY + usFontGetH(pubFont) * 3 / 4 - 1;


  for(yy=0; yy<h; yy++) {
      for(xx=0; xx<w; xx++) {
          if(!(bit++ & 7)) {
            bits = pgm_read_byte(&bitmap[bo++]);
          }
          if(bits & 0x80) {
            tft->drawPixel(x+xo+xx, y+yo+yy, usColor);
          }
          bits <<= 1;
      }
  }

}


static void vHLine(uint16_t usX0, uint16_t usY0, uint16_t usX1, uint16_t usColor) {
  if (!tft)
    return;

  if (usX0 == usX1) usX1++;
  tft->drawFastHLine(usX0, usY0, usX1 - usX0, usColor);
}

static void vVLine(uint16_t usX0, uint16_t usY0, uint16_t usY1, uint16_t usColor) {
  if (!tft)
    return;
  if (usY0 == usY1) usY1++;
  tft->drawFastVLine(usX0, usY0, usY1 - usY0, usColor);
}

static void bPicture(int16_t sX0, int16_t sY0, xPicture pusPicture) {
  if (!tft || !bmFile)
    return;

  Serial.println(pusPicture);
  if (bmFile->open(pusPicture)){

    uint32_t W = bmFile->width();
    uint32_t H = bmFile->height();

    uint32_t sX1 = sX0 + W - 1;
    uint32_t sY1 = sY0 + H - 1;

    uint32_t dataSize = W * 2;
    auto padding = (4 - dataSize % 4) % 4;
    dataSize  = dataSize + padding; // in case of odd width we should pad to 4 bytes
    dataSize *= H;

    tft->setRotation(5);
    //we need to translate coords with respect to MAXIMUM available resolution for our controller
    //tft->setWindow(sX0, ILI9341_TFTWIDTH - sY1, sX1, ILI9341_TFTHEIGHT - sY0);
    tft->setWindow(sX0, 240 - sY1, sX1, 240 - sY0);
    //for swapping X and Y
    //tft->setWindow(EMGUI_LCD_WIDTH - sX1, ILI9341_TFTHEIGHT -  sY1, EMGUI_LCD_WIDTH - sX0, ILI9341_TFTHEIGHT - sY0);

    tft->writecommand(ILI9341_RAMWR);

    bmFile->seekData();
    char data[32]; //TODO: move from stack! 
    auto bufSize = sizeof(data);
    char * p = data;
    uint32_t btw = 0;

    if (padding) { //we split two algos by duplicating code to skip runtime `if(pads)` checking for every pixel
      uint32_t rowPixels = W;
      while ((btw = bmFile->readBytes(p, (dataSize > bufSize)?bufSize:dataSize )) > 0){
        // Serial.print("Read: ");
        // Serial.println(btw);
        dataSize -= btw;
        char * pEnd = data + (btw / 2) * 2; // read only even numbers of bytes
        while (p < pEnd){
          if ( rowPixels-- == 0 ) { //skip padding at the end of the row
            p+= padding;
            rowPixels = W;
            continue;
          }

          tft->writedata(*(p+1));
          tft->writedata(*p);
          p+= 2;
        }
        p = data;
      }
    } else {
      while ((btw = bmFile->readBytes(p, (dataSize > bufSize)?bufSize:dataSize )) > 0){
        dataSize -= btw;
        char * pEnd = data + (btw / 2) * 2; // read only even numbers of bytes
        while (p < pEnd){
          tft->writedata(*(p+1));
          tft->writedata(*p);
          p+= 2;
        }
        p = data;
      }
    }

    //tft->writecommand (ILI9341_MADCTL);
    //tft->writedata(0x00);
    tft->setRotation(1);
  }
}

static uint16_t usGetPictureW(xPicture pusPicture) {
  if (!bmFile)
    return 0;

  if (bmFile->open(pusPicture))
    return  bmFile->width();

  return 0;
}

static uint16_t usGetPictureH(xPicture pusPicture) {
  if (!bmFile)
    return 0;

  if (bmFile->open(pusPicture))
    return  bmFile->height();

  return 0;
}

static const GFXfont * xDefaultFont[] = { &FreeSans9pt7b, &MyriadPro_Regular9pt8b, NULL };

xFont xGetDefaultFont() {
  return xDefaultFont;
}

void TFTSleep(){
  if (!tft)
      return;

  tft->writecommand(ILI9341_SLPIN);
}

void TFTWake(){
  if (!tft)
      return;

  tft->writecommand(ILI9341_SLPOUT);
}

void vGUIGlueInit(){
  // Config EM_GUI opts 
  EMGUI_LCD_WIDTH = 320;
  EMGUI_STATUS_BAR_HEIGHT = 26;
  if (!tft){
    SPIFFS.begin();
    bmFile = new BMPFile();
    tft =  new TFT_ILI9341_ESP();  // Use hardware SPI

    vDrawHandlerInit(&LCD);
    LCD.vRectangle = &vRectangle;
    LCD.vPutChar = &vPutChar;
    LCD.bPicture = &bPicture;
    LCD.vVLine = &vVLine;
    LCD.vHLine = &vHLine;
    LCD.usGetPictureH = &usGetPictureH;
    LCD.usGetPictureW = &usGetPictureW;

    LCD.usFontGetH  = &usFontGetH;
    LCD.ucFontGetCharW = &ucFontGetCharW;
    LCD.usFontGetStrW  = &usFontGetStrW;
    LCD.usFontGetStrH  = &usFontGetStrH;
    LCD.xGetDialogPictureSet = prvGetPicSet;
    LCD.xGetDefaultFont = xGetDefaultFont;

    vDrawSetHandler(&LCD);

    tft->init();
    tft->fillScreen(TFT_LIGHTGREY);
    tft->setRotation(1);
  }
}