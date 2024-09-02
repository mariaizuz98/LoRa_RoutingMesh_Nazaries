#ifndef ROUTING_H
#define ROUTING_H

#include "Arduino.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "config/config.h"
#include "package/package.h"
#include "lcd/lcd.h"

#define MAX_TABLE_SIZE 10
#define GATEWAY_ID       0x8C

typedef struct{
    byte        destinationAddress;
    byte        nextHop;
    uint        hopCount;
    char        sequenceRoute[20];
} routeTableEntry;

void setupRoutingTable              (void);
void resetRoutingTable              (void);
void sendRREQ                       (byte destinationId);
void sendRREP                       (byte destinationId, const char* incomingSequence);
void sendRRER                       (byte destinationId);
void analyzeSeqRoute                (int rssi, const char *sequenceRoute);
void evaluateAndSelectBestRoute     (void);
int getTotalHopToGateway            (const char *sequenceRoute);
void updateRouteTable               (const char *sequenceRoute);
char* getNextHop                    (const char *sequenceRoute);

#endif