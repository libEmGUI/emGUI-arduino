#include <SPI.h>
#include <Adafruit_ILI9341.h>

#define TFT_CS   D8
#define TFT_DC   D2
#define RESET    0 

#define USE_TF13012A

#ifdef USE_TF13012A
  // In our example we use screen TF13012A which has different init sequence, so we must redefine it with our sequence
  class TFT_TF13012A : public Adafruit_ILI9341{
    public:
    TFT_TF13012A(int8_t cs, int8_t dc, int8_t rst):Adafruit_ILI9341(cs, dc, rst){}
    void      begin(uint32_t freq = 0);
  };
  
  TFT_TF13012A tft = TFT_TF13012A(TFT_CS, TFT_DC, RESET);
#else
  Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
#endif



void setup() {
  Serial.begin(9600);
  Serial.println("Program started"); 
  tft.begin(40000000);

}


void loop(void) {
  tft.fillScreen(ILI9341_BLACK);
  tft.fillScreen(ILI9341_RED);
  tft.fillScreen(ILI9341_GREEN);
  tft.fillScreen(ILI9341_BLUE);
}

#define SPI_DEFAULT_FREQ         40000000

 void TFT_TF13012A::begin(uint32_t freq)
 {

    if(!freq){
        freq = SPI_DEFAULT_FREQ;
    }
    _freq = freq;

    // Control Pins
    pinMode(_dc, OUTPUT);
    digitalWrite(_dc, LOW);
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);

    // Software SPI
    if(_sclk >= 0){
        pinMode(_mosi, OUTPUT);
        digitalWrite(_mosi, LOW);
        pinMode(_sclk, OUTPUT);
        digitalWrite(_sclk, HIGH);
        if(_miso >= 0){
            pinMode(_miso, INPUT);
        }
    }

    // Hardware SPI
    SPI_BEGIN();
 
   digitalWrite(TFT_RST, HIGH);
   delay(1); //Delay 1ms
   digitalWrite(TFT_RST, LOW);
   delay(10); //Delay 10ms
   digitalWrite(TFT_RST, HIGH);
   delay(120); //Delay 120ms
   //---------------------------------------------------------------------------------------------------//
   writeCommand (0x11);
   delay(120); //Delay 120ms 
 
   // DATASHEET SEQUENCE
   writeCommand (0x36);
   spiWrite(0x00);
   writeCommand (0x3a);
   spiWrite(0x05);
   writeCommand (0x21);
   writeCommand (0x2a);
   spiWrite(0x00);
   spiWrite(0x00);
   spiWrite(0x00);
   spiWrite(0xef);
   writeCommand (0x2b);
   spiWrite(0x00);
   spiWrite(0x00);
   spiWrite(0x00);
   spiWrite(0xef);
   //--------------------------------ST7789V Frame ratesetting----------------------------------//
   writeCommand (0xb2);
   spiWrite(0x0c);
   spiWrite(0x0c);
   spiWrite(0x00);
   spiWrite(0x33);
   spiWrite(0x33);
   writeCommand (0xb7);
   spiWrite(0x35); 
   writeCommand (0xbb);
   spiWrite(0x1f);
   writeCommand (0xc0);
   spiWrite(0x2c);
   writeCommand (0xc2);
   spiWrite(0x01);
   writeCommand (0xc3);
   spiWrite(0x12);
   writeCommand (0xc4);
   spiWrite(0x20);
   writeCommand (0xc6);
   spiWrite(0x0f);
   writeCommand (0xd0);
   spiWrite(0xa4);
   spiWrite(0xa1);
   //--------------------------------ST7789V gammasetting--------------------------------------//
   writeCommand (0xe0);
   spiWrite(0xd0);
   spiWrite(0x08);
   spiWrite(0x11);
   spiWrite(0x08);
   spiWrite(0x0c);
   spiWrite(0x15);
   spiWrite(0x39);
   spiWrite(0x33);
   spiWrite(0x50);
   spiWrite(0x36);
   spiWrite(0x13);
   spiWrite(0x14);
   spiWrite(0x29);
   spiWrite(0x2d);
   writeCommand (0xe1);
   spiWrite(0xd0);
   spiWrite(0x08);
   spiWrite(0x10);
   spiWrite(0x08);
   spiWrite(0x06);
   spiWrite(0x06);
   spiWrite(0x39);
   spiWrite(0x44);
   spiWrite(0x51);
   spiWrite(0x0b);
   spiWrite(0x16);
   spiWrite(0x14);
   spiWrite(0x2f);
   spiWrite(0x31);
   writeCommand (0x29);
 }

