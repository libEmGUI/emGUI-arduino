#ifndef _APDS_WRAPPER_
#define _APDS_WRAPPER_

#include "APDS9930.h"

//APDS Proximity config
#define APDS9930_INT                INTR_SYS    // Needs to be an interrupt pin

#define PROX_INT_THRESHOLD_DISABLED 0 
#define APDS_INT_THRESHOLD_NEAR     450 //the more pulses we get, the closer is the object
#define APDS_INT_THRESHOLD_FAR      350

#if APDS_INT_THRESHOLD_NEAR < APDS_INT_THRESHOLD_FAR
#error FAR threshold should be less or equal to NEAR one
#endif

class APDSWrapper : public APDS9930 {
public:

  enum {
    AVALID = 1 << 0,
    PVALID = 1 << 1,
    AINT =   1 << 4,
    PINT =   1 << 5,
    PSAT =   1 << 6,
  } STATUS_BITS;

  typedef enum 
  {
    APDS_STATE_UNKNOWN,
    APDS_STATE_NEAR,
    APDS_STATE_FAR,
    APDS_STATE_ERROR
  } State;

  bool enable();

  void disable();

  void loop();

  bool isFailed() {
    return _errCount >= _errCountThreshold;
  };
protected:
  const static int _errCountThreshold = 50;
  uint32_t _errCount = 0;
  bool _enabled = false;
  bool getIntStatus();
};



#endif //_APDS_WRAPPER_
