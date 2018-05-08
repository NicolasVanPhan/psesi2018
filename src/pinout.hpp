/**
 * 
 * pinout.hpp
 * 
 * Project name     : PSESI - Autonomous Vehicle for cartography
 * Author           : Nicolas Phan <nicolas.van.phan@gmail.com>
 * Creation date    : 04/22/18
 * 
 * This header file contains the pin mapping for Arduino Mega.
 * To prevent mapping conflicts betweent the different functions of the project,
 * one should use ONLY the macros defined below for specifying pins when calling
 * functions like digitalWrite(), analogRead()...
 * 
 */
#ifndef H_PINOUT
#define H_PINOUT

#define DC_ID       0
#define BT_ID       1

/* ------------------------- DC Motor Mapping ------------------------------- */
#define PIN_EN12    22
#define PIN_1A      5
#define PIN_2A      6
#define PIN_EN34    23
#define PIN_3A      7
#define PIN_4A      8
#define PIN_FWHEEL  9  /* Using Servo library disables pins 44, 45 and 46 */
#define PIN_POT     A0
#define PIN_BTN     25

/* ------------------------- Bluetooth module mapping ----------------------- */
#define PIN_BT_RX  19   /* If you change these pins, you should change */
#define PIN_BT_TX  18   /* Serial1 to SerialX in bluetooth.cpp */

#endif
