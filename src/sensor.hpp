/**
 *
 * capteur.hpp
 *
 * Project name     : PSESI - Autonomous Vehicule for cartography
 * Author           : Bryan Aggoun <bryan.aggoun@gmail.com>
 * Creation date    : 04/28/18
 *
 * This header file contains the prototypes of all the functions
 * used to interact with the ultrasonic sensor
 *
 */

#ifndef SENSOR_H
#define SENSOR_H

#include <SPI.h>
#include <Servo.h>
#include "pinout.hpp"

#define   US_ANGLE_OFFSET   95

void  US_init();
long  US_read();

void  US_set_angle(int angle);
int   US_get_angle();

#endif
