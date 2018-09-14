#ifndef _PERIPHERIAL_H_
#define _PERIPHERIAL_H_


#include <Wire.h>
#include <Arduino.h>
#include <stdint.h>
#include "Extender.h"
//#include "GUI.h"
#include <memory>
#include "src/SimpleTimer/SimpleTimer.h"
#include "APDSWrapper.h"
#include "Battery.hpp"
#include "PushPop.hpp"
#include "TouchWrapper.h"


class Peripherial;
class SingletonDestroyer
{
  private:
    Peripherial* p_instance;
  public:
    ~SingletonDestroyer();
    void initialize( Peripherial* p );
};


class Peripherial: public Extender {
  private:
    static Peripherial* p_instance;
    static SingletonDestroyer destroyer;

    Peripherial(uint8_t addr = ADDR0): Extender(addr) {}
    ~Peripherial() {} 

    // необходимо также запретить копирование
    Peripherial(Peripherial const&); // реализация не нужна
    Peripherial& operator= (Peripherial const&);  // и тут

    friend class SingletonDestroyer;

    PushPop _button0 = PushPop();
    PushPop _button1 = PushPop();
    SimpleTimer _vibro;
    volatile bool _intrFlag;
    volatile bool _intrFlagNFC;
    uint16_t _batVoltage;
    APDSWrapper _apds = APDSWrapper();             // Proximity sensor
	bool _sleep = true;
	bool isSleeping = false;
  public:
    static Peripherial* getInstance(); 


    typedef enum {
      EV_BTN_PRESS,      //param is button number
      EV_BTN_RELEASE,    //param is button number
	  EV_BTN_LONG,
      EV_NFC,            //param is void
      EV_CHRG_STATE,     //param is chargeStatus_t
      EV_VOLTAGE_CHANGE, //param is voltage
      EV_PROXIMITY,
    } eventType;

    typedef void(*onEvent_t)(Peripherial *, eventType, int32_t param1);

    enum {
      PE_VIBRO      = (1 << 0),   // PIN 0
      PE_BL         = (1 << 1),   // PIN 1
      PE_BTN0       = (1 << 2),   // PIN 2
      PE_BTN1       = (1 << 3),   // PIN 3      
      PE_CTP_PWR    = (1 << 4),   // PIN 4
      PE_CHRG_CH    = (1 << 5),   // PIN 5
      PE_CHRG_STB   = (1 << 6),   // PIN 6
      PE_DCDC_SLEEP = (1 << 7),   // PIN 7

    } PE_PINS;


    void init(){
      static bool done = false;
      if (!done){
        enableIntAsOD();  // IntPin is OD active low
        enableSEQOP();
        configAsInputs((uint8_t) ~(PE_VIBRO | PE_BL | PE_CTP_PWR | PE_DCDC_SLEEP));                        // These are outputs, everething else is input
        setInternalPU(PE_CHRG_CH | PE_CHRG_STB );            // These have internal pullups
        enableIntForPins(PE_BTN1 | PE_BTN0 | PE_CHRG_CH | PE_CHRG_STB );  // These have interrupts enabled
        touchPower(true);
        dcdcActive(true);
		wifi = false;

        _apds.disablePower();
        _apds.enable();
        _intrFlagNFC = false;
        done = true;
        bat.init();
		touch.init();
      }
    }

	void powerSaveExit();
	void powerSaveEnter();
	void sleep(bool sl);
	void enableSleep(bool state);

    void reset(){
      backlight(false);
      touchPower(false);
      dcdcActive(false);
      LCD(false);
      _apds.disable();
      ESP.reset();
    }

    void powerOff(){
      backlight(false);
      touchPower(false);
      dcdcActive(false);
      LCD(false);
      _apds.disable();
      ESP.deepSleep(0);
    }

    void powerTest(int delay){
      backlight(false);
      touchPower(false);
      dcdcActive(false);
      LCD(false);
      _apds.disable();
      ESP.deepSleep(delay, RF_CAL);
      powerSaveExit();
    }

	void wakeUp() {
		sleepTimeout.reset();
		sleep(false);
	}

	Timeout sleepTimeout = Timeout(10000);
	TouchWrapper touch;
    bool loop(bool sysInt, bool nfcInt);
    void ICACHE_RAM_ATTR intr();
    void ICACHE_RAM_ATTR intrNFC();
    void checkChargeStatus();

    void LCD(bool newState);
    void touchPower(bool newState);
    void backlight(bool newState);
    void vibro(bool newState);
    void dcdcActive(bool newState);

	bool wifi;
    //actions
    void vibroPulse(uint16_t delay_ms = 300);

    onEvent_t onEvent = NULL;
    void endVibro();

    APDSWrapper * getProximity(){
      return &_apds;
    }

    Battery bat = Battery(PE_CHRG_CH, PE_CHRG_STB);     // config pins
};

#endif //_PERIPHERY_H_
