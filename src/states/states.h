#ifndef STATES_H
#define STATES_H

// General libraries 
#include "Arduino.h"
#include <stdio.h>
// Others libraries
#include "config/config.h"
#include "package/package.h"
#include "sensors/sensor_DHT.h"
#include "routing/routing.h"
#include "lcd/lcd.h"

#define EVENT_BIT_ROUTING       1
#define EVENT_BIT_ROUTE_ERROR   2

/********************  Functions  *******************/
void switchStates      (void);
void sendDATA          (byte neighborID);
void sendDATANeighbor  (byte neighborID, const char* incomingMeasureNeighbor);
/****************************************************/

#endif