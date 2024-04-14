#include "states.h"

enum states {
    STANDBY,
    READY,
    LINKED,
    WAITING,
} state;

char measure[256]; // Arreglo estático para almacenar la medida

extern hw_timer_t *sendTimer, *responseTimer;
extern bool sendLoRa, sendLoRaAgain, recieveACK;
extern RouteInfo routingTable[MAX_NODES];

void switchStates(void){
    switch (state) {
        case STANDBY:
            state = READY;
            break; 
        case READY:
            // Implementa la lógica de estado READY si es necesario
            sendPackage(BROADCAST, ROUTING, NULL);
            state = LINKED;
            break;
        case LINKED:
            if(sendLoRa){
                char* dataMeasure = readSensorDHT();
                strncpy(measure, dataMeasure, sizeof(measure) - 1);
                measure[sizeof(measure) - 1] = '\0';
                sendPackage(routingTable->nextHop, DATA, measure);
                sendLoRa = false; 

                Serial.println("Hola 1");
                state = WAITING;
            }
            break;
        case WAITING:
            if(sendLoRaAgain){
                sendPackage(routingTable->nextHop, DATA, measure);
                sendLoRaAgain = false;

                Serial.println("Hola 2");
            } else if(recieveACK){
                timerStop(responseTimer);
                timerStart(sendTimer);
                recieveACK = false;

                Serial.println("Hola 3");
                state = LINKED;
            }
            break;
        default:
            break;
    }
}
