/**
 * 
 * bluetooth.hpp
 * 
 * Project name     : PSESI - Autonomous Vehicle for cartography
 * Author           : Nicolas Phan <nicolas.van.phan@gmail.com>
 * Creation date    : 04/22/18
 * 
 * This header file contains the prototypes for all the functions
 * used to control the bluetooth communication module (HC-05).
 * 
 * Take a look at the example functions to see how to use the API
 * 
 */
#ifndef H_BLUETOOTH
#define H_BLUETOOTH

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include "pinout.hpp"

#define BT_MSG_END_CHAR   10    // NL new line character

/* ----------- Example functions -------------------------------------------- */
void    BT_example();
void    BT_example2();

/* ----------- Bluetooth API ------------------------------------------------ */
int     BT_available();
String  BT_read();
void    BT_print(String msg);
void    BT_print(char msg);
void    BT_print(int msg);
void    BT_print(unsigned int msg);
void    BT_print(long msg);
void    BT_print(unsigned long msg);
void    BT_init();    // This should appear in setup()
void    BT_loop();    // This should appear in loop()

#endif
