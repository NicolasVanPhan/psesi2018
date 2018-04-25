/**
 * 
 * pinout.hpp
 * 
 * Project name     : PSESI - Autonomous Vehicle for cartography
 * Author           : Nicolas Phan <nicolas.van.phan@gmail.com>
 * Creation date    : 04/22/18
 * 
 * This header file contains the pin mapping for Arduino Uno and Mega.
 * To prevent mapping conflicts betweent the different functions of the project,
 * one should use ONLY the macros defined below for specifying pins when calling
 * functions like digitalWrite(), analogRead()...
 * 
 */
#ifndef H_PINOUT
#define H_PINOUT

/* Uncomment the line corresponding to the board you want to use */
#define USE_UNO_BOARD
/* #define USE_MEGA_BOARD */

/* -------------------------------------------------------------------------- */
/* ------------------------- Mapping for Arduino Uno ------------------------ */
/* -------------------------------------------------------------------------- */
#ifdef USE_UNO_BOARD

/* ------------------------- DC Motor Mapping ------------------------------- */
#define PIN_EN12    8
#define PIN_1A      5
#define PIN_2A      6
#define PIN_EN34    7
#define PIN_3A      3
#define PIN_4A      11
#define PIN_FWHEEL  9

#endif

/* -------------------------------------------------------------------------- */
/* ------------------------- Mapping for Arduino Mega ----------------------- */
/* -------------------------------------------------------------------------- */
#ifdef USE_MEGA_BOARD

/* ------------------------- DC Motor Mapping ------------------------------- */

#endif

#endif
