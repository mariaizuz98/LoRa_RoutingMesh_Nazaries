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

/********************  Functions  *******************/
void switchStates      (void);
void sendDATA          (void);
/****************************************************/

#endif