/**********************************************************************

DCCpp_Uno.h
COPYRIGHT (c) 2013-2016 Gregg E. Berman

Part of DCC++ BASE STATION for the Arduino

**********************************************************************/

#include "Config.h"

#ifndef DCCpp_Uno_h
#define DCCpp_Uno_h

/////////////////////////////////////////////////////////////////////////////////////
// RELEASE VERSION
/////////////////////////////////////////////////////////////////////////////////////

#define VERSION "1.2.1+"

/////////////////////////////////////////////////////////////////////////////////////
// AUTO-SELECT ARDUINO BOARD
/////////////////////////////////////////////////////////////////////////////////////

#ifdef ARDUINO_AVR_MEGA                   // is using Mega 1280, define as Mega 2560 (pinouts and functionality are identical)
  #define ARDUINO_AVR_MEGA2560
#endif

#if defined  ARDUINO_AVR_UNO

  #define ARDUINO_TYPE    "UNO"

  #define DCC_SIGNAL_PIN_MAIN 10          // Ardunio Uno  - uses OC1B
  #define DCC_SIGNAL_PIN_PROG 5           // Arduino Uno  - uses OC0B

  #if COMM_INTERFACE != 0                 // Serial was not selected

    #error CANNOT COMPILE - DCC++ FOR THE UNO CAN ONLY USE SERIAL COMMUNICATION - PLEASE SELECT THIS IN THE CONFIG FILE

  #endif

#elif defined  ARDUINO_AVR_MEGA2560

  #define ARDUINO_TYPE    "MEGA"

  #define DCC_SIGNAL_PIN_MAIN 12          // Arduino Mega - uses OC1B
  #define DCC_SIGNAL_PIN_PROG 2           // Arduino Mega - uses OC3B

#elif defined  ARDUINO_AVR_NANO

  #define ARDUINO_TYPE    "NANO"

  #define DCC_SIGNAL_PIN_MAIN 10          // Arduino Nano - uses OC1B
  #define DCC_SIGNAL_PIN_PROG 5           // Arduino Nano - uses OC0B

  #define POWER_MANAGER_NOT_ALIVE_PIN 8
  #define POWER_MANAGER_WAKEUP_PIN    2
  #define POWER_MANAGER_LED_PIN       7
  #define POWER_MANAGER_REBOOT_PIN    9
  #define POWER_STARTUP_MIN_DURATION_SECONDS   20
  #define POWER_SHUTDOWN_MIN_DURATION_SECONDS  12

#else

  #error CANNOT COMPILE - DCC++ ONLY WORKS WITH AN ARDUINO UNO OR AN ARDUINO MEGA 1280/2560

#endif

/////////////////////////////////////////////////////////////////////////////////////
// SELECT MOTOR SHIELD
/////////////////////////////////////////////////////////////////////////////////////

#if MOTOR_SHIELD_TYPE == 0

  #define MOTOR_SHIELD_NAME "ARDUINO MOTOR SHIELD"

  #define SIGNAL_ENABLE_PIN_MAIN 3
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 12
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 13

  #define CURRENT_SAMPLE_MAX            300
  #define ACK_SAMPLE_THRESHOLD          30      // the threshold that the exponentially-smoothed analogRead samples (after subtracting the baseline current) must cross to establish ACKNOWLEDGEMENT  

  #define ENABLE_PIN_MAIN_LEVEL_ON      HIGH
  #define ENABLE_PIN_MAIN_LEVEL_OFF     LOW
  #define ENABLE_PIN_PROG_LEVEL_ON      HIGH
  #define ENABLE_PIN_PROG_LEVEL_OFF     LOW


#elif MOTOR_SHIELD_TYPE == 1

  #define MOTOR_SHIELD_NAME "POLOLU MC33926 MOTOR SHIELD"

  #define SIGNAL_ENABLE_PIN_MAIN 9
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 7
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 8

  #define CURRENT_SAMPLE_MAX            300
  #define ACK_SAMPLE_THRESHOLD          30      // the threshold that the exponentially-smoothed analogRead samples (after subtracting the baseline current) must cross to establish ACKNOWLEDGEMENT  

  #define ENABLE_PIN_MAIN_LEVEL_ON      HIGH
  #define ENABLE_PIN_MAIN_LEVEL_OFF     LOW
  #define ENABLE_PIN_PROG_LEVEL_ON      HIGH
  #define ENABLE_PIN_PROG_LEVEL_OFF     LOW

#elif MOTOR_SHIELD_TYPE == 2

  #define MOTOR_SHIELD_NAME "SEBTRONIC DCC-STATION MOTOR SHIELD"

  #define SIGNAL_ENABLE_PIN_MAIN 3
  #define SIGNAL_ENABLE_PIN_PROG 11

  #define DRIVER_FAULT_PIN_MAIN 6
  #define DRIVER_FAULT_PIN_PROG 4

  #define CURRENT_MONITOR_PIN_MAIN A0
  #define CURRENT_MONITOR_PIN_PROG A1

  #define DIRECTION_MOTOR_CHANNEL_PIN_A 12
  #define DIRECTION_MOTOR_CHANNEL_PIN_B 13

  // Max current with LMD18200 is 3A. By security, we accept 2.5A.
  // ACS712 give 0.185V/A. Full range (0-1023) is for 0-5V input.
  // So max current is 0.185 * 2.5 * 1023 / 5 = 94,6275
  #define  CURRENT_SAMPLE_MAX           95

  // Threshold is defined by DCC specifications as at least 60mA for 60ms. Use 50mA for margin.
  // ACS712 give 0.185V/A. Full range (0-1023) is for 0-5V input.
  // So max current is 0.185 * 0.05 * 1023 / 5 = 1,89255
  #define  ACK_SAMPLE_THRESHOLD         2

  #define ENABLE_PIN_MAIN_LEVEL_ON      LOW
  #define ENABLE_PIN_MAIN_LEVEL_OFF     HIGH
  #define ENABLE_PIN_PROG_LEVEL_ON      HIGH
  #define ENABLE_PIN_PROG_LEVEL_OFF     LOW

#else

  #error CANNOT COMPILE - PLEASE SELECT A PROPER MOTOR SHIELD TYPE

#endif

/////////////////////////////////////////////////////////////////////////////////////
// SELECT COMMUNICATION INTERACE
/////////////////////////////////////////////////////////////////////////////////////

#if COMM_INTERFACE == 0

  #define COMM_TYPE 0
  #define INTERFACE Serial

#elif (COMM_INTERFACE==1) || (COMM_INTERFACE==2) || (COMM_INTERFACE==3)

  #define COMM_TYPE 1
  #define INTERFACE eServer
  #define SDCARD_CS 4
  
#else

  #error CANNOT COMPILE - Please select a proper value for COMM_INTERFACE in CONFIG.H file

#endif

/////////////////////////////////////////////////////////////////////////////////////
// SET WHETHER TO SHOW PACKETS - DIAGNOSTIC MODE ONLY
/////////////////////////////////////////////////////////////////////////////////////

// If SHOW_PACKETS is set to 1, then for select main operations track commands that modify an internal DCC packet register,
// if printFlag for that command is also set to 1, DCC++ BASE STATION will additionally return the 
// DCC packet contents of the modified register in the following format:

//    <* REG: B1 B2 ... Bn CSUM / REPEAT>
//
//    REG: the number of the main operations track packet register that was modified
//    B1: the first hexidecimal byte of the DCC packet
//    B2: the second hexidecimal byte of the DCC packet
//    Bn: the nth hexidecimal byte of the DCC packet
//    CSUM: a checksum byte that is required to be the final byte in any DCC packet
//    REPEAT: the number of times the DCC packet was re-transmitted to the tracks after its iniital transmission
 
#define SHOW_PACKETS  0       // set to zero to disable printing of every packet for select main operations track commands

/////////////////////////////////////////////////////////////////////////////////////

#endif


