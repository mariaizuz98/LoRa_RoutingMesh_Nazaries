#ifndef SENSOR_DHT_H
#define SENSOR_DHT_H

#include "Arduino.h"
#include <DHT.h>

#ifdef TTGO_LORA_V1
    #define DHTPIN 16   
    #define DHTTYPE DHT11  
#elif TTGO_LORA_V21    
    #define DHTPIN 4   
    #define DHTTYPE DHT11   
#endif 

void setupDHT(void);
char* readSensorDHT(void);

#endif