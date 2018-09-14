#include "Peripherial.h"



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
bool Peripherial::loop() {

  bool passed = false;

  return passed;
}

