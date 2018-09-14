#pragma once

#include <stdint.h>
#include <pgmspace.h>
#include "Wire.h"

void I2CBitbanger(uint8_t sda, uint8_t sck) {
	pinMode(sda, OUTPUT);
	pinMode(sck, OUTPUT);
	digitalWrite(sck, 0);
	digitalWrite(sda, 0);
	
	//TODO:  trigger event
	uint8_t counter = 0;
	pinMode(sda, INPUT_PULLUP);
	pinMode(sck, INPUT_PULLUP);

	delay (100);
	if (digitalRead(sck) == 1){
	bool busError = false;

	delay (10);
	if ( digitalRead(sda) == 0 ){
	  delay (10);
	  Serial.println(F("I2C SDA is LOW, bumping clock...."));
	  
	  while ( digitalRead(sda) == 0 ){
		Serial.print(F("^"));
		pinMode(sck, OUTPUT);
		digitalWrite(sck, 0);
		delay(10);
		pinMode(sck, INPUT_PULLUP);
		delay(10);
		counter++;
		yield();
		if (counter > 64) {
		  Serial.println(F("Unable to bump I2C bus..."));
		  pinMode(sda, OUTPUT);
		  digitalWrite(sda, 1);
		  delay(10);
		  pinMode(sda, INPUT_PULLUP);
		  
		  break; // TODO reset here
		}
	  }
	}

	if(  (digitalRead(sda) == 0) ||  (digitalRead(sck) == 0) ){
	  busError = true;
	  Serial.println(F("Unable to recover I2C bus..."));
	}
	if ( !busError ){
	  Serial.println(F("Sending I2C start/stop sequence"));
	  pinMode(sda, OUTPUT); //start
	  delay(1);
	  pinMode(sck, OUTPUT);
	  delay(1);
	  pinMode(sda, INPUT_PULLUP); //stop
	  delay(1);
	  pinMode(sck, INPUT_PULLUP);
	  delay(1);
	}
	} else {
		Serial.println(F("I2C SCK is LOW, unable to recover!"));
	}
	Wire.begin(sda, sck);
}
