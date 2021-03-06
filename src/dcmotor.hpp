/**
 * 
 * dcmotor.hpp
 * 
 * Project name     : PSESI - Autonomous Vehicle for cartography
 * Author           : Nicolas Phan <nicolas.van.phan@gmail.com>
 * Creation date    : 04/22/18
 * 
 * This header file contains the prototypes for all the functions
 * used to control of the DC motors.
 * 
 * Take a look at the example functions to see how to use the API
 * 
 */
#ifndef H_DCMOTOR
#define H_DCMOTOR

#include "pinout.hpp"
#include "waitfor.hpp"
#include <SPI.h>
#include <Wire.h>
#include <Servo.h>

#define DC_MAX_SPEED    255.0
#define DC_MAX_ANGLE    90

#define DCSTATE_WAITCMD           0
#define DCSTATE_TURNWHEEL         1
#define DCSTATE_TURNWHEEL_DELAY   2

#define DC_TURNDELAY    50
#define DC_TURNPACE     5.0

/* ----------- Example functions -------------------------------------------- */
void    example1();   // control the vehicule through the USB serial

/* ----------- API 1 : Raw control over the Motors -------------------------- */
void    DC_Init ();
int     DC_SetLeftSpeed (int param_speed);
int     DC_SetRightSpeed (int param_speed);
int     DC_SetWheelAngle (int angle);
void    DC_Refresh ();

/* ----------- API 2 : Commande the vehicle move ---------------------------- */
void    DC_forward();
void    DC_backward();
void    DC_left();
void    DC_right();
void    DC_stop();
void    DC_loop(); // that function should always run

/* ------------------------- Internal functions ----------------------------- */

#endif
