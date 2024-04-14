#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

extern bool receiveMsg;

void setup(void){
    Serial.begin(9600);
    while (!Serial.availableForWrite()) {
      delay(10);
    }
    
    config_Init();
    #ifdef NODE_LORA
      create_taskStates();
    #endif
    create_taskListen();
}

void loop(void){
    // if(LoRa.parsePacket() != 0) readPackage;
}

void create_taskStates(void){
    xTaskCreatePinnedToCore(vTaskStates, "vTaskStates", STACK_SIZE, NULL, 1, NULL, ARDUINO_RUNNING_CORE0);
}

void create_taskListen(void){
    xTaskCreatePinnedToCore(vTaskListen, "vTaskListen", STACK_SIZE, NULL, 1, NULL, ARDUINO_RUNNING_CORE1);
}

void vTaskListen( void *pvParameters ){
    while(1){
        if(recievePackage()) readPackage();
        receiveMsg = false;
        // if(LoRa.parsePacket() != 0) readPackage;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void vTaskStates( void *pvParameters ){
    while(1){
        switchStates();
        vTaskDelay(pdMS_TO_TICKS(200)); // 200 OK
    }
}