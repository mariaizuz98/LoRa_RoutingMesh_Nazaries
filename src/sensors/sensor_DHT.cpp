#include "sensor_DHT.h"

DHT dht(DHTPIN, DHTTYPE);
float humidity, tempT = 0;
char data;

void setupDHT(void){
    dht.begin();
    Serial.println("* Initializing DHT11... DHT11 OK");
}

char* readSensorDHT(void){
    humidity = dht.readHumidity() * 10;
    tempT = dht.readTemperature() * 10;
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(tempT)) {
        #ifdef TTGO_LORA_V1
            Serial.println(" --- Error: failed to read from DHT11 sensor!");
        #elif TTGO_LORA_V21
            Serial.println(" --- Error: failed to read from DHT22 sensor!");
        #endif
        humidity = 0;
        tempT = 0;
        sprintf(&data,""); 
        return &data;
    }
    Serial.printf("* Temperature (ºC):  %.2f  |  Humidity:  %.2f\r\n", tempT/10, humidity/10);
    sprintf(&data, "%d;%d", (int)tempT, (int)humidity);
    return &data;
}