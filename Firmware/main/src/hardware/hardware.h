// this file holds definitions of all the hardware configurations

#ifndef _CL_HW_HARDWARE_H_
#define _CL_HW_HARDWARE_H_

#include "Arduino.h"
#include "config.h"

#ifdef CL_HW_13R
#define MISSING0 5
#define MISSING1 0
#define MISSING2 0
#define MISSING3 0
#define MISSING4 0
#define MISSING5 0
#define MISSING6 0
#define MISSING7 0
#define MISSING8 0
#define MISSING9 0
#define MISSING10 1
#define MISSING11 8

#define X_LEDS (20)
#define Y_LEDS (12)

#define MISSING_LEDS (MISSING0 + MISSING1 + MISSING2 + MISSING3 + MISSING4 + MISSING5 + MISSING6 + MISSING7 + MISSING8 + MISSING9 + MISSING10 + MISSING11)
#endif

#ifdef CL_HW_13NR
#define MISSING0 4
#define MISSING1 0
#define MISSING2 0
#define MISSING3 0
#define MISSING4 0
#define MISSING5 0
#define MISSING6 0
#define MISSING7 0
#define MISSING8 0
#define MISSING9 0
#define MISSING10 0
#define MISSING11 4
#define MISSING12 7

#define X_LEDS (21)
#define Y_LEDS (13)

#define MISSING_LEDS (MISSING0 + MISSING1 + MISSING2 + MISSING3 + MISSING4 + MISSING5 + MISSING6 + MISSING7 + MISSING8 + MISSING9 + MISSING10 + MISSING11 + MISSING12)
#endif

#ifdef CL_HW_ET_PROTOTYPE
#define MISSING0 5
#define MISSING1 0
#define MISSING2 0
#define MISSING3 0
#define MISSING4 0
#define MISSING5 0
#define MISSING6 0
#define MISSING7 0
#define MISSING8 0
#define MISSING9 0
#define MISSING10 0
#define MISSING11 1
#define MISSING12 7

#define X_LEDS (21)
#define Y_LEDS (13)

#define MISSING_LEDS (MISSING0 + MISSING1 + MISSING2 + MISSING3 + MISSING4 + MISSING5 + MISSING6 + MISSING7 + MISSING8 + MISSING9 + MISSING10 + MISSING11 + MISSING12)
#endif


// don't change these
#define NUM_LEDS (X_LEDS * Y_LEDS) - MISSING_LEDS
extern int preArray[Y_LEDS][X_LEDS];
extern uint8_t colorIndex[Y_LEDS][X_LEDS];
extern int ledArray[Y_LEDS][X_LEDS];

#endif // _CL_HW_HARDWARE_H_
