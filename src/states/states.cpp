#include "states.h"

enum states {
    READY,
    LINKED,
    WAITING,
} state;

char measure[256]; // Arreglo estático para almacenar la medida
int cntResend = 0;
byte local_neighbor;

extern byte localID;
extern hw_timer_t *sendTimer, *responseTimer;
extern bool sendLoRa, sendLoRaAgain, recieveACK;
extern routeTableEntry routeTable;
extern EventGroupHandle_t eventSend, eventRouting;
extern TaskHandle_t taskHandleStates;
extern TaskHandle_t taskHandleListen;

void switchStates(void){
    switch (state) {
        case READY:
            // xEventGroupWaitBits(eventSend, EVENT_BIT_SEND, pdTRUE, pdFALSE, portMAX_DELAY);
            if(sendLoRa){
                if(routeTable.nextHop != 0){
                    state = LINKED;
                } else {                  
                    sendRREQ(routeTable.destinationAddress);
                }
                xEventGroupWaitBits(eventRouting, EVENT_BIT_ROUTING, pdTRUE, pdFALSE, 10000/portTICK_PERIOD_MS);
            }
            // if(routeTable.nextHop != 0){
            //     state = LINKED;
            // } else {                      
            //     sendRREQ(routeTable.destinationAddress);
            // }
            // xEventGroupWaitBits(eventRouting, EVENT_BIT_ROUTING, pdTRUE, pdFALSE, 1000/portTICK_PERIOD_MS);
            break;
        case LINKED:
            // xEventGroupWaitBits(eventSend, EVENT_BIT_SEND, pdTRUE, pdFALSE, portMAX_DELAY);
            // xEventGroupClearBits(eventSend, EVENT_BIT_SEND);
            if(sendLoRa){
                cntResend = 0;
                sendDATA(localID);
                sendLoRa = false; 
                state = WAITING;
            }
            // cntResend = 0;
            // sendDATA();
            // state = WAITING;
            break;
        case WAITING:
            if(sendLoRaAgain){
                if(cntResend > 5){
                    sendRRER(BROADCAST);
                    timerStop(responseTimer);
                    timerStart(sendTimer);
                    sendLoRa = true;
                    sendLoRaAgain = false;

                    state = READY;
                } else {   
                    cntResend++;
                    Serial.printf(" --> Se envia de nuevo el paquete con los datos. Contador de reenvio: %d \r\n", cntResend);
                    sendPackage(localID, local_neighbor, routeTable.nextHop, DATA, measure);
                    sendLoRaAgain = false;
                }

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

void sendDATA(byte localID){
    local_neighbor = localID;

    char* dataMeasure = readSensorDHT();
    strncpy(measure, dataMeasure, sizeof(measure) - 1);
    measure[sizeof(measure) - 1] = '\0';
    sendPackage(localID, localID, routeTable.nextHop, DATA, measure);
    timerStart(responseTimer);
}

void sendDATANeighbor(byte neighborID, const char* incomingMeasureNeighbor){
    char measureNeighbor[256]; // Arreglo estático para almacenar la medida
    local_neighbor = neighborID;
    
    strncpy(measureNeighbor, incomingMeasureNeighbor, sizeof(measure) - 1);
    measure[sizeof(measure) - 1] = '\0';
    sendPackage(localID, neighborID, routeTable.nextHop, DATA, measureNeighbor);  
    timerStop(sendTimer);
    timerStart(responseTimer);    

    state = WAITING;
}
