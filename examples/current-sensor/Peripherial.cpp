#include "Peripherial.h"
#include "emGUIGlue.h"
#include "TouchWrapper.h"

Peripherial * Peripherial::p_instance = 0;
SingletonDestroyer Peripherial::destroyer;
  
SingletonDestroyer::~SingletonDestroyer() {   
    delete p_instance; 
}
void SingletonDestroyer::initialize( Peripherial* p ) {
    p_instance = p; 
}
Peripherial* Peripherial::getInstance() {
    if(!p_instance) {
        p_instance = new Peripherial();
        destroyer.initialize( p_instance);
    }
    return p_instance;
}


/*********************** main loop of periph ***********************/
bool Peripherial::loop(bool sysInt, bool nfcInt) {
  _vibro.run();
  bool passed = false;
  bat.loop();
  if (_button0.isLongPressed() == PushPop::LONGPRESS) {
    if (!isSleeping) onEvent(this, EV_BTN_LONG, 0);
    passed = true;
  }
  if (_button1.isLongPressed() == PushPop::LONGPRESS) {
    if (!isSleeping) onEvent(this, EV_BTN_LONG, 1);
    passed = true;
  }
  if (_intrFlag || sysInt) {
	 
    //vGUIHandlePeriph(EV_HW_SYS_INT, 0);
    _intrFlag = false;
    auto int_state = readFromExtender(GPIO);
    auto charge_state = ~int_state & (PE_CHRG_STB | PE_CHRG_CH);

	if (bat.checkStatus(charge_state)) {
		onEvent(this, EV_CHRG_STATE, bat.status);
		passed = true;
	}
	

    if (int_state & PE_BTN0) {
      if (_button0.push() == PushPop::PUSHED){
        if (!isSleeping) onEvent(this, EV_BTN_PRESS, 0);
        passed = true;
      } 
    } else {
       _button0.pop();
    }

    if (int_state & PE_BTN1) {
		if (_button1.push() == PushPop::PUSHED) {
			if (!isSleeping) onEvent(this, EV_BTN_PRESS, 1);
			passed = true;
		}
	}
	else {
      _button1.pop();
	}

	if (int_state & (PE_BTN0 | PE_BTN1)) sleepTimeout.reset();
  }
  if (_intrFlagNFC || nfcInt) {
    _intrFlagNFC = false;
    onEvent(this, EV_NFC, 1);
    passed = true;
  }
  
  _apds.loop();

  if (sleepTimeout.isOver()) {
	  sleep(true);
  }
  else {
	  sleep(false);
  }
  return passed;
}

void ICACHE_RAM_ATTR Peripherial::intr() {
  _intrFlag = true;
}

void ICACHE_RAM_ATTR Peripherial::intrNFC() {
  _intrFlagNFC = true;
}


/***************************** actions *****************************/
// Do the brrr-brrr
void Peripherial::vibroPulse(uint16_t delay_ms) {
  Serial.println("Brrrrr");
  setPins(PE_VIBRO, true);
  _vibro.setTimeout(delay_ms, std::bind(&Peripherial::endVibro, this));
}

void Peripherial::endVibro() {
  setPins(PE_VIBRO, false);
}

void Peripherial::LCD(bool newState){
  if (newState){
    TFTWake();
  } else {
    TFTSleep();
  }
}

void Peripherial::touchPower(bool newState){
  setPins(PE_CTP_PWR, !newState);

  if(newState)
    touch.init();
}

void Peripherial::backlight(bool newState){
  setPins(PE_BL, newState);
}

void Peripherial::vibro(bool newState){
  setPins(PE_VIBRO, newState);
}

void Peripherial::dcdcActive(bool newState){
  setPins(PE_DCDC_SLEEP, newState);
}

void Peripherial::sleep(bool sl) {
	if (!_sleep) return;
	if (sl == isSleeping) return;

	isSleeping = sl;
	if (sl) {
		powerSaveEnter();
	}
	else {
		powerSaveExit();
	}

}

void Peripherial::powerSaveExit() {
	touchPower(true);
	backlight(true);
	LCD(true);
	isSleeping = false;
}


void Peripherial::powerSaveEnter() {
	touchPower(false);
	backlight(false);
	LCD(false);
	isSleeping = true;
}

void Peripherial::enableSleep(bool state) {
	_sleep = state;

}