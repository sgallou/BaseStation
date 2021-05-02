/**********************************************************************

CurrentMonitor.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#ifndef CurrentMonitor_h
#define CurrentMonitor_h

#include "Arduino.h"

#define CURRENT_SAMPLE_SMOOTHING 0.01

#if defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_NANO // Configuration for UNO / NANO
#define CURRENT_SAMPLE_TIME 10
#else // Configuration for MEGA
#define CURRENT_SAMPLE_TIME 1
#endif

class CurrentMonitor
{
public:
  CurrentMonitor(int pin, int faultPin, const char *msg);
  void calibrateCurrentSensor();
  float currentRefValue();
  static boolean checkTime();
  void check();
  float load();
  void resetErrorState();

private:
  static long int sampleTime;
  int pin;
  int faultPin;
  float current;
  float currentRef = 0.0;
  const char *msg;
  bool errorState = false;
};

#endif
