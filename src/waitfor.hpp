/**
 * 
 * waitfor.hpp
 * 
 * Project name     : PSESI - Autonomous Vehicle for cartography
 * Author           : Nicolas Phan <nicolas.van.phan@gmail.com>
 * Creation date    : 04/25/18
 * 
 * This header file contains the wait_for() function,
 * a non-blocking equivalent to the delay() function intended to
 * be used in a FSM, in a multitasking context.
 * 
 */
#ifndef H_WAITFOR
#define H_WAITFOR

#include <SPI.h>
#include <Wire.h>

#define WAITFOR_MAX_TIMER   10

int   wait_for(int id, unsigned long duration);

#endif
