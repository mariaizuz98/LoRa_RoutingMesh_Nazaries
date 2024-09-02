#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/event_groups.h>
#include "config/config.h"
#include "package/package.h"
#include "states/states.h"

#define STACK_SIZE              4096
#define ARDUINO_RUNNING_CORE0   0
#define ARDUINO_RUNNING_CORE1   1

void create_taskStates(void);
void create_taskListen(void);
void vTaskListen( void * pvParameters );
void vTaskStates( void * pvParameters );

// Declaración del grupo de eventos
EventGroupHandle_t eventSend, eventRouting;

// Handle para cada tarea 
TaskHandle_t taskHandleStates = NULL;
TaskHandle_t taskHandleListen = NULL;

extern bool receiveMsg;

void setup(void){
    Serial.begin(9600);
    while (!Serial.availableForWrite()) {
      delay(10);
    }
    config_Init();

    eventSend = xEventGroupCreate();
    eventRouting = xEventGroupCreate();

    #ifdef NODE_LORA
      create_taskStates();
    #endif
    create_taskListen();
}

void loop(void){
    // if(LoRa.parsePacket() != 0) readPackage;
}

void create_taskStates(void){
    xTaskCreatePinnedToCore(vTaskStates, "vTaskStates", STACK_SIZE, NULL, 1, &taskHandleStates, ARDUINO_RUNNING_CORE0);
}

void create_taskListen(void){
    xTaskCreatePinnedToCore(vTaskListen, "vTaskListen", STACK_SIZE, NULL, 1, &taskHandleListen, ARDUINO_RUNNING_CORE1);
}

void vTaskListen( void *pvParameters ){
    while(1){
        if(recievePackage()) readPackage();
        receiveMsg = false;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vTaskStates( void *pvParameters ){
    while(1){
        switchStates();
        vTaskDelay(pdMS_TO_TICKS(200)); // 200 OK
    }
}