#ifndef HTTP_H
#define HTTP_H

#include "Arduino.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Time.h>
#include "mbedtls/md.h"      // HMAC hashing used for the pairing process
#include "mbedtls/base64.h"  // Base64 encoding/decoding for the paring proces

void updateTime     (void);
void newJSON        (int t, int h);
void encryptData    (void);
void SendPost       (void);


#endif