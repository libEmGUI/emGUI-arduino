#ifndef _PERIPHERIAL_H_
#define _PERIPHERIAL_H_


#include <Wire.h>
#include <Arduino.h>
#include <stdint.h>


#include <memory>
#include "src/TouchWrapper/TouchWrapper.h"


class Peripherial;
class SingletonDestroyer
{
  private:
    Peripherial* p_instance;
  public:
    ~SingletonDestroyer();
    void initialize( Peripherial* p );
};


class Peripherial {
  private:
    static Peripherial* p_instance;
    static SingletonDestroyer destroyer;

    Peripherial() {}
    ~Peripherial() {} 

    // необходимо также запретить копирование
    Peripherial(Peripherial const&); // реализация не нужна
    Peripherial& operator= (Peripherial const&);  // и тут

    friend class SingletonDestroyer;

    volatile bool _intrFlag;
    volatile bool _intrFlagNFC;
    uint16_t _batVoltage;

    bool _sleep = true;
    bool isSleeping = false;
  public:
    static Peripherial* getInstance(); 


    typedef enum {
      EV_BTN_PRESS,      //param is button number
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
        done = true;
		    touch.init();
      }
    }

	  TouchWrapper touch;
    bool loop();

    onEvent_t onEvent = NULL;
};

#endif //_PERIPHERY_H_
