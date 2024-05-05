#ifndef PACKAGE_H
#define PACKAGE_H

// Generial libreries 
#include "Arduino.h"
#include "config/config.h"
#include "states/states.h"
#include "http/http.h"
#include "routing/routing.h"

/* ID DE MENSAJES */
#define RREQ                        0x01
#define RREP                        0x02
#define ACK                         0x04
#define DATA                        0x08
// #define NOT_READY                   0x10

/* LISTA DE ID GENERICOS */
#define BROADCAST                   0xFF
#define GATEWAY                     0x8C

/********************  Functions  *******************/
void sendPackage            (byte destID, byte msgID, char* msg);
bool recievePackage         (void);
void readPackage            (void);
void identifyActionLoRa     (byte msgID);
void sendDataToCloud        (void);
void parseSensorData        (const char* sensorData);
/****************************************************/

#endif