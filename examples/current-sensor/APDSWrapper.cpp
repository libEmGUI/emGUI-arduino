#include "APDSWrapper.h"
#include "User_Setup.h"
#include "GUI.h"

bool APDSWrapper::getIntStatus(){
  uint8_t status;

  if ( digitalRead(APDS9930_INT) != 0 )
    return false;

  // TODO: check this, might be useless!
  // sensor is not in sync with system's state after deep sleep or reset, 
  // it can be configured in previous startup and hold an interrupt low
  // so, we need to ignore enabled flag if it is low and INT is triggered
  if ( !_enabled && !isFailed() )
    return true;   

  if( !wireReadDataByte(APDS9930_STATUS, status) )
    return false;

  if ( status & PINT )
    return true;
  
  return false;
}

void APDSWrapper::disable() { 
    
  //TODO: trigger an event!

  if( !disablePower() ){
    Serial.println(F("Error disabling proximity sensor"));
  }

  _enabled = false; 
};

bool APDSWrapper::enable() {
  if ( _enabled )
    return true;

  if ( !APDS9930::init() ) {
    Serial.println(F("Something went wrong during APDS-9930 init!"));
    return false;
  }

  
  // Configure sensor to NEAR event trigger
  if ( !setProximityIntLowThreshold(APDS_INT_THRESHOLD_FAR) ) {
    Serial.println(F("Error writing low threshold"));
    return false;
  }

  if ( !setProximityIntHighThreshold(APDS_INT_THRESHOLD_NEAR) ) {
    Serial.println(F("Error writing high threshold"));
    return false;
  }

  // Start running the APDS-9930 proximity sensor (interrupts)
  if ( !enableProximitySensor(true) ) {
    Serial.println(F("Error enabling proximity sensor!"));
    return false;
  }

  // Adjust the Proximity sensor gain
  if ( !setProximityGain(PGAIN_1X) ) {
    Serial.println(F("Unable to set PGAIN"));
    return false;
  }

  if ( !setLEDDrive(LED_DRIVE_12_5MA) ) {
    Serial.println(F("Unable to set setLEDDrive"));
    return false;
  }

  _enabled = true;
  return true;
}


void APDSWrapper::loop() {

  if ( getIntStatus() ) {

    auto low = getProximityIntLowThreshold();
    auto high =  getProximityIntHighThreshold();

    State state = APDS_STATE_UNKNOWN;

    if (low == APDS_INT_THRESHOLD_FAR && high == PROX_INT_THRESHOLD_DISABLED)
      state = APDS_STATE_FAR;
    else if (low == PROX_INT_THRESHOLD_DISABLED && high == APDS_INT_THRESHOLD_NEAR)
      state = APDS_STATE_NEAR;
    else if (low == APDS_INT_THRESHOLD_FAR && high == APDS_INT_THRESHOLD_NEAR) {

      uint16_t proximity = 0;

      Serial.println(F("Default values detected... Checking proximity value!"));

      if( !readProximity(proximity)){
        Serial.println(F("Unable to readProximity"));
        _errCount++; 
      }

      if ( proximity >= APDS_INT_THRESHOLD_NEAR )
        state = APDS_STATE_NEAR;
      else
        state = APDS_STATE_FAR;

    } else {
      state = APDS_STATE_ERROR;
      _errCount++; 
    }

    switch (state){
    case APDS_STATE_NEAR:
      Serial.println(F("ON_NEAR"));
      vGUIHandlePeriph(EV_HW_PROXIMITY, 0);
      low = APDS_INT_THRESHOLD_FAR;
      high = PROX_INT_THRESHOLD_DISABLED;
    break;
    case APDS_STATE_FAR:
      Serial.println(F("ON_FAR"));
      vGUIHandlePeriph(EV_HW_PROXIMITY, 1);
      low = PROX_INT_THRESHOLD_DISABLED;
      high = APDS_INT_THRESHOLD_NEAR;
    break;
    default:
      Serial.println(F("Invalid proximity thresholds detected! Trying to fix..."));
      low = APDS_INT_THRESHOLD_FAR;
      high = APDS_INT_THRESHOLD_NEAR;
    break;
    }

    if ( !setProximityIntLowThreshold(low) ) {
      Serial.println(F("Error writing low threshold"));
      _errCount++; 
    }

    if ( !setProximityIntHighThreshold(high) ) {
      Serial.println(F("Error writing high threshold"));
      _errCount++; 
    }

    if ( !clearProximityInt() ) {
      Serial.println(F("Error clearing interrupt"));
      _errCount++; 
    }

    if ( isFailed() ){
      //TODO: On sensor error!
      disable(); //Seems like sensor is doing bananas, do not waste a time and power on it
    }
  }
}
