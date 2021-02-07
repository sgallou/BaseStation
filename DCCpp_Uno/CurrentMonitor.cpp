/**********************************************************************

CurrentMonitor.cpp
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "DCCpp_Uno.h"
#include "CurrentMonitor.h"
#include "Comm.h"

///////////////////////////////////////////////////////////////////////////////

CurrentMonitor::CurrentMonitor(int pin, const char *msg){
    this->pin=pin;
    this->msg=msg;
    current=0;
  } // CurrentMonitor::CurrentMonitor

void CurrentMonitor::calibrateCurrentSensor(){
   // Current reference, 0 for positive only current sensors (MAX471...)
                          // Something like half-range (512) for symetric current sensor (ACS712...)
                          // Can be optained by calibration at startup

#if MOTOR_SHIELD_TYPE == 0
    currentRef = 0.0;
#elif MOTOR_SHIELD_TYPE == 1
    currentRef = 0.0;
#elif MOTOR_SHIELD_TYPE == 2

  // Be sure outputs are configured and motor drivers are off
  pinMode(SIGNAL_ENABLE_PIN_PROG,OUTPUT);
  pinMode(SIGNAL_ENABLE_PIN_MAIN,OUTPUT);
  digitalWrite(SIGNAL_ENABLE_PIN_PROG,ENABLE_PIN_PROG_LEVEL_ON);
  digitalWrite(SIGNAL_ENABLE_PIN_MAIN,ENABLE_PIN_MAIN_LEVEL_ON);
  pinMode(pin, INPUT);

  // Calibration
  currentRef = 0.0;
  int loop = 100;
  do
  {
    currentRef += analogRead(pin);
    delay(1);
  }
  while (--loop);
  currentRef /= 100;
  Serial.println(currentRef);
#endif
}
  
boolean CurrentMonitor::checkTime(){
  if(millis()-sampleTime<CURRENT_SAMPLE_TIME)            // no need to check current yet
    return(false);
  sampleTime=millis();                                   // note millis() uses TIMER-0.  For UNO, we change the scale on Timer-0.  For MEGA we do not.  This means CURENT_SAMPLE_TIME is different for UNO then MEGA
  return(true);  
} // CurrentMonitor::checkTime
  
void CurrentMonitor::check(){
  current=abs(analogRead(pin) - currentRef)*CURRENT_SAMPLE_SMOOTHING+current*(1.0-CURRENT_SAMPLE_SMOOTHING);        // compute new exponentially-smoothed current
  if(current>CURRENT_SAMPLE_MAX && digitalRead(SIGNAL_ENABLE_PIN_PROG)==ENABLE_PIN_PROG_LEVEL_ON){                    // current overload and Prog Signal is on (or could have checked Main Signal, since both are always on or off together)
    digitalWrite(SIGNAL_ENABLE_PIN_PROG,ENABLE_PIN_PROG_LEVEL_OFF);                               // disable both Motor Shield Channels
    digitalWrite(SIGNAL_ENABLE_PIN_MAIN,ENABLE_PIN_MAIN_LEVEL_OFF);                               // regardless of which caused current overload
    INTERFACE.print(msg);                                                                            // print corresponding error message
  }    
} // CurrentMonitor::check  

long int CurrentMonitor::sampleTime=0;

