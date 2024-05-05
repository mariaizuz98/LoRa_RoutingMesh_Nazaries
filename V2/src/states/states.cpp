#include "states.h"

enum states {
    CONFIG_PERIPHERAL,
    READY,
    LINKED,
    WAITING,
} state;

char measure[256]; // Arreglo estático para almacenar la medida

extern hw_timer_t *sendTimer, *responseTimer;
extern bool sendLoRa, sendLoRaAgain, recieveACK;
extern routeTableEntry routeTable;

void switchStates(void){
    switch (state) {
        case CONFIG_PERIPHERAL:
            state = READY;
            break; 
        case READY:
            if(sendLoRa){
                if(routeTable.nextHop != 0){
                    sendDATA();
                    sendLoRa = false;   
                    state = WAITING;
                }     
                else{                        
                    sendRREQ(GATEWAY_ID);
                    delay(5000);
                    //while(routeTable.nextHop == 0);
                }
            }
            break;
        case LINKED:
            if(sendLoRa){
                sendDATA();
                sendLoRa = false; 
                state = WAITING;
            }
            break;
        case WAITING:
            if(sendLoRaAgain){
                sendPackage(GATEWAY_ID, DATA, measure);
                sendLoRaAgain = false;

            } else if(recieveACK){
                timerStop(responseTimer);
                timerStart(sendTimer);
                recieveACK = false;

                state = LINKED;
            }
            break;
        default:
            break;
    }
}

void sendDATA(void){
    char* dataMeasure = readSensorDHT();
    strncpy(measure, dataMeasure, sizeof(measure) - 1);
    measure[sizeof(measure) - 1] = '\0';
    sendPackage(routeTable.nextHop, DATA, measure);
    timerStart(responseTimer);
}
