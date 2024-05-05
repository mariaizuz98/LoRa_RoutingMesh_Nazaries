#ifndef CONFIG_H
#define CONFIG_H

// Generial libreries 
#include "Arduino.h"
// LoRa libraries
#include <SPI.h>
#include <LoRa.h>
// OLED Display libraries for 
#include "SSD1306.h"
#include "lcd/lcd.h"
// WiFi and ThingSpeak libreries
#ifdef GATEWAY_LORA
    #include "secrets/secrets.h"
    #include <WiFi.h>
    #include <ThingSpeak.h>
    extern WiFiClient client;
#endif
// Others libraries
#include "sensors/sensor_DHT.h"
#include "timers/timer.h"

//433E6 for Asia ; 868E6 for Europe; 915E6 for North America
#define BAND 868E6
#define TXPOWER 10
/***************  Configuration Pins  ***************/
#ifdef TTGO_LORA_V1
    // OLED V1
    #define OLED_SDA    4
    #define OLED_SCL    15
    #define OLED_RST    16
    #define OLED_WIDTH  128  // OLED display width, in pixels
    #define OLED_HEIGHT 64  // OLED display height, in pixels
    // LORA V1
    #define LORA_SCK    5
    #define LORA_MISO   19
    #define LORA_MOSI   27
    #define LORA_CS     18
    #define LORA_RST    14
    #define LORA_DIO0   26
#endif

#ifdef TTGO_LORA_V21
    // OLED V2.1_1.6
    #define OLED_SDA    21
    #define OLED_SCL    22
    #define OLED_RST    -1
    #define OLED_WIDTH  128  // OLED display width, in pixels
    #define OLED_HEIGHT 64  // OLED display height, in pixels
    // LORA V2.1_1.6
    #define LORA_SCK    5
    #define LORA_MISO   19
    #define LORA_MOSI   27
    #define LORA_CS     18
    #define LORA_RST    23
    #define LORA_DIO0   26
#endif
/****************************************************/
#define ERRNO      -1

/********************  Functions  *******************/
int config_Init         (void);
void setupID            (void);
int setupDisplay        (void);
int setupLORA           (void);
void setupWiFi          (void);
/****************************************************/

#endif