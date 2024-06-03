#include "states.h"

enum states {
    READY,
    LINKED,
    WAITING,
} state;

char measure[256]; // Arreglo estático para almacenar la medida
int cntResend = 0;

extern hw_timer_t *sendTimer, *responseTimer;
extern bool sendLoRa, sendLoRaAgain, recieveACK;
extern routeTableEntry routeTable;

void switchStates(void){
    switch (state) {
        case READY:
            if(sendLoRa){
                if(routeTable.nextHop != 0){
                    state = LINKED;
                } else {                        
                    sendRREQ(GATEWAY_ID);
                    delay(2000);
                    //while(routeTable.nextHop == 0);
                }
            }
            break;
        case LINKED:
            if(sendLoRa){
                cntResend = 0;
                sendDATA();
                sendLoRa = false; 
                state = WAITING;
            }
            break;
        case WAITING:
            if(sendLoRaAgain){
                if(cntResend > 5){
                    state = READY;
                    routeTable.nextHop = 0;
                } else {   
                    cntResend++;
                    Serial.printf(" --> Se envia de nuevo el paquete con los datos. Contador de reenvio: %d \r\n", cntResend);
                    sendPackage(routeTable.nextHop, DATA, measure);
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

void sendDATA(void){
    char* dataMeasure = readSensorDHT();
    strncpy(measure, dataMeasure, sizeof(measure) - 1);
    measure[sizeof(measure) - 1] = '\0';
    sendPackage(routeTable.nextHop, DATA, measure);
    timerStart(responseTimer);
}
