/*     

USER DEFINED SETTINGS

http://www.forward.com.au/pfod/ESP8266/GPIOpins/index.html

 We must use hardware SPI for Touch and Display

| NodeMCU  |      Function       |  Display        | INA 219 (optional)     | Comment                                   |
| ---------|---------------------|-----------------|------------------------| ------------------------------------------|
| D1       |                     | T_CS            |                        | To Display                                |
| D2       |                     | CS              |                        | To Display                                |
| D3       | [BOOT, I2C] SDA     |                 | SDA                    | To INA219                                 |
| D4       | [BOOT, I2C] SCL     |                 | SCL                    | To INA219, Should be HIGH on startup      |
| D5       | [SPI] SCK           | SCK, T_CLK      |                        | To Display                                |
| D6       | [SPI] MISO          | SDO, T_DO       |                        | To Display                                |
| D7       | [SPI] MOSI          | SDI, T_DI       |                        | To Display                                |
| D8       | [BOOT]              | DC              |                        | To Display, Should be LOW on startup      |
| RST      | [RESET]             | RST             |                        | To Display, Optional                      |
| GND      | [GROUND]            | GND             | GND                    | To INA and Display                        |
| VCC      | [+3,3V]             | VCC, LED        | VCC                    | To INA and Display                        |

*/

// ###### EDIT THE PIN NUMBERS IN THE FOLLOWING BLOCK TO SUIT YOUR SETUP ######

#define TFT_DC   D8  // Data Command control pin
#define TFT_CS   D2 
#define TOUCH_CS D1
#define TFT_RST -1   // Use system reset or opt-out

#define SDA_PIN   D3
#define SCL_PIN   D4

// #define SPI_FREQUENCY 20000000
#define SPI_FREQUENCY 40000000
// #define SPI_FREQUENCY 80000000
// #define SPI_FREQUENCY 16000000


// Comment out the following #define if "SPI Transactions" do not need to be
// supported. Tranaction support is required if other SPI devices are connected.
// When commented out the code size will be smaller and sketches will
// run slightly faster, so leave it commented out unless you need it!
// Transaction support is needed to work with SD library but not needed with TFT_SdFat

// #define SUPPORT_TRANSACTIONS

