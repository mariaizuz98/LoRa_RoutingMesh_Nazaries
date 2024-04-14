#include "timer.h"

hw_timer_t *sendTimer = NULL;
hw_timer_t *responseTimer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

bool sendLoRa = false;
bool sendLoRaAgain = false;

void IRAM_ATTR onSendTimer(void){
    portENTER_CRITICAL(&timerMux);
    timerStart(responseTimer);
    timerStop(sendTimer);
    timerWrite(sendTimer, 0);
    sendLoRa = true;  
    portEXIT_CRITICAL(&timerMux);  
}

void IRAM_ATTR onResponseTimer(void){
    portENTER_CRITICAL(&timerMux);
    sendLoRaAgain = true;
    portEXIT_CRITICAL(&timerMux);
}


int setupTimers(void){
    // Timer1 para la interrupción del envio de mensajes mediante LoRa
    // Timer2 para la interrupción de vuelta a enviar 
    Serial.print("* Initializing Timers...");
    sendTimer = timerBegin(0, 80, true);
    responseTimer = timerBegin(1, 80, true);

    if (sendTimer == NULL || responseTimer == NULL) {
        Serial.println(" --- Error: timer not created");
        return -1;
    }

    timerAttachInterrupt(sendTimer, &onSendTimer, true);
    timerAttachInterrupt(responseTimer, &onResponseTimer, true);
    timerAlarmWrite(sendTimer, TIME_SEND_MSG, true);  // Seteado para auto-reload
    timerAlarmWrite(responseTimer, TIME_RESPONSE_MSG, true);  // Seteado para auto-reload

    timerAlarmEnable(sendTimer);  // Habilita solo el timer de envío inicialmente
    timerAlarmEnable(responseTimer);  // Asegura que el timer de respuesta esté deshabilitado inicialmente
    timerStop(responseTimer);  
    timerWrite(responseTimer, 0);

    Serial.println(" Timers OK");
    return 0;
}
