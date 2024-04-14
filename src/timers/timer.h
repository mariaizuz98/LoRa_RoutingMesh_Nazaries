#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"
#include "package/package.h"

// #define TIME_SEND_MSG        900000000           // alarma cada 15 min (valor en microsegundos)
// #define TIME_RESPONSE_MSG    120000000           // alarma cada 2 min (valor en microsegundos)
#define TIME_SEND_MSG        120000000           // alarma cada 2 min (valor en microsegundos) PARA PRUEBAS
#define TIME_RESPONSE_MSG    60000000            // alarma cada 1 min (valor en microsegundos) PARA PRUEBAS

// extern const uint8_t GATEWAY_ID;

void IRAM_ATTR  onSendTimer(void);
void IRAM_ATTR  onResponseTimer(void);
int             setupTimers(void);

#endif