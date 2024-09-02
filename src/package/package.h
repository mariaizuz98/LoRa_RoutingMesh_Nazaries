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
#define ACK                         0x03
#define DATA                        0x04
#define RERR                        0x05

/* LISTA DE ID GENERICOS */
#define BROADCAST                   0xFF
#define GATEWAY                     0x8C

/********************  Functions  *******************/
void sendPackage            (byte senderID, byte senderNeighborID, byte destID, byte msgID, char* msg);
// void sendPackageNeighbor    (byte destID, byte msgID, byte neighborID, char* msg);
bool recievePackage         (void);
void readPackage            (void);
void identifyActionLoRa     (byte msgID);
void sendDataToCloud        (void);
void parseSensorData        (const char* sensorData);
/****************************************************/

#endif