/*     USER DEFINED SETTINGS

http://www.forward.com.au/pfod/ESP8266/GPIOpins/index.html

 We must use hardware SPI for Touch and Display

Display GND      to NodeMCU pin GND (0V)
Display VCC      to NodeMCU pin VIN (5V)
Display CS       to NodeMCU pin D2
Display RESET    unconnected
Display SDI/MOSI to NodeMCU pin D7
Display SCK      to NodeMCU pin D5
Display LED      to NodeMCU pin VIN (5V)
Display SDO/MISO to NodeMCU pin D6

D0   = 16;  - internal PULL-DOWN  
D1   = 5;   - *TOUCH CS*
D2   = 4;   - *TFT CS*
D3   = 0;   - [BOOT] *I2C SDA_PIN*
D4   = 2;   - [BOOT] Should be HIGH on startup *SPI SCK_PIN*
D5   = 14;  - [I2C] I2C SCK
D6   = 12;  - [SPI] SDO/MISO
D7   = 13;  - [SPI] SDI/MOSI
D8   = 15;  - [BOOT] should be LOW on startup *TFT DC*
D9   = 3;   - [UART] rx
D10  = 1;   - [UART] tx
*/

// ###### EDIT THE PIN NUMBERS IN THE 3 LINES FOLLOWING TO SUIT YOUR SETUP ######

#define TFT_DC   D8  // Data Command control pin
#define TFT_CS   D2 
#define TOUCH_CS D1
#define TFT_RST -1

#define SDA_PIN   D3
#define SCK_PIN   D5

// #define SPI_FREQUENCY 20000000
// #define SPI_FREQUENCY 40000000
 #define SPI_FREQUENCY 40000000
// #define SPI_FREQUENCY 16000000


// Comment out the following #define if "SPI Transactions" do not need to be
// supported. Tranaction support is required if other SPI devices are connected.
// When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!
// Transaction support is needed to work with SD library but not needed with TFT_SdFat

// #define SUPPORT_TRANSACTIONS

