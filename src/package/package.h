#ifndef PACKAGE_H
#define PACKAGE_H

// Generial libreries 
#include "Arduino.h"
#include "config/config.h"
#include "states/states.h"
#include "http/http.h"

/* ID DE MENSAJES */
// #define REQUEST_CONNECT             0x00
#define CONNECT_TO_GATEWAY          0x01
#define ROUTING                     0x02
#define ACK                         0x04
#define DATA                        0x08
// #define NOT_READY                   0x10

/* LISTA DE ID GENERICOS */
#define BROADCAST                   0xFF
#define GATEWAY                     0x8C

/********************  Functions  *******************/
void sendPackage            (byte destID, byte msgID, char* msg);
bool recievePackage         (void);
// void sendPackage(uint16_t msg, byte destination);
// void sendPackageACK(byte destination);
// void readPackage(int packetSize);
void readPackage            (void);
void identifyActionLoRa     (byte msgID);
void processRoutingMessage  (uint32_t from, int rssi);
void sendDataToCloud        (void);
void parseSensorData        (const char* sensorData);
/****************************************************/

#endif