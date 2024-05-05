#ifndef ROUTING_H
#define ROUTING_H

#include "Arduino.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <LoRa.h>
#include "config/config.h"
#include "package/package.h"

#define MAX_TABLE_SIZE 10
#define GATEWAY_ID       0x8C

typedef struct{
    byte destinationAddress;
    byte nextHop;
    int hopCount;
    char sequenceRoute[20];
} routeTableEntry;

void setupRoutingTable      (void);
void sendRREQ               (byte destinationId);
void sendRREP               (byte destinationId, char* incomingSequence);
void updateRouteTable       (const char *sequenceRoute);
char* getNextHop            (const char *sequenceRoute);

#endif