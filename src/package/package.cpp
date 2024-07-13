#include "package.h"

extern byte localID;
extern SSD1306 display;
extern hw_timer_t *sendTimer;
extern hw_timer_t *responseTimer;
extern routeTableEntry routeTable;

byte senderID;          // sender address
byte receiverID;          // recipient address
byte incomingMsgID;     // incoming msg ID
String incomingMsg;     // data sensor
int incomingTempT , incomingHumidity = 0;
bool receiveMsg = false;
bool recieveACK = false;

void sendPackage(byte destID, byte msgID, char* msg){
    // send packet
    LoRa.beginPacket();
    LoRa.write(localID);
    LoRa.write(destID);
    LoRa.write(msgID);
    if (msg != NULL) {
        LoRa.print(msg);  // Only send the message if it's not NULL
    }
    LoRa.endPacket();

    String msgID_char;
    if(msgID == 1)          msgID_char = "RREQ";
    else if(msgID == 2)     msgID_char = "RREP";
    else if(msgID == 3)     msgID_char = "ACK";
    else if(msgID == 4)     msgID_char = "DATA";

    Serial.printf(" --- Message LoRa send %s --- \r\n", msgID_char);
    Serial.printf("Sender: 0x%2X |  Destination: 0x%2X  |  Message ID: %s  |  Message: %s  \r\n", 
                    localID, destID, msgID_char, msg ? msg : "NULL");
}

bool recievePackage (void){
    if(LoRa.parsePacket() != 0)     receiveMsg = true;
    return receiveMsg;
}

void readPackage(void){
    // read packet header bytes: 
    senderID = LoRa.read();                             // sender address
    receiverID = LoRa.read();                           // recipient/gateway address
    incomingMsgID = LoRa.read();                        // incoming msg ID
    incomingMsg = "";

    while (LoRa.available()) {
        incomingMsg += (char)LoRa.read();
    }
    
    identifyActionLoRa(incomingMsgID);
}

void identifyActionLoRa(byte msgID){
    switch (msgID){
        case RREQ:
            Serial.println(" --- Message LoRa receive RREQ --- ");
            Serial.printf("Sender: 0x%2X |  Destination: 0x%2X  |  Message ID: RREQ  |  Message: %s  |  RSSI:  %d  | SNR:  %.2f  \r\n", 
                            senderID, receiverID, incomingMsg, LoRa.packetRssi(), LoRa.packetSnr()); 
            sendRREP(senderID, strdup(incomingMsg.c_str()));
            break;
        case RREP:
            if(receiverID == localID){
                Serial.println(" --- Message LoRa receive RREP --- ");
                Serial.printf("Sender: 0x%2X |  Destination: 0x%2X  |  Message ID: RREP  |  Message: %s  |  RSSI:  %d  | SNR:  %.2f  \r\n", 
                                senderID, receiverID, incomingMsg, LoRa.packetRssi(), LoRa.packetSnr()); 
                if(LoRa.packetRssi() > -120 && senderID == GATEWAY_ID){
                    updateRouteTable(strdup(incomingMsg.c_str()));
                }
            }
            break;
        case DATA:
            if(receiverID == localID){
                Serial.println(" --- Message LoRa receive DATA --- ");
                Serial.printf("Sender: 0x%2X |  Destination: 0x%2X  |  Message ID: DATA  |  Message: %s  |  RSSI:  %d  | SNR:  %.2f  \r\n", 
                                senderID, receiverID, incomingMsg, LoRa.packetRssi(), LoRa.packetSnr()); 
                #ifdef GATEWAY_LORA
                    sendDataToCloud();
                #endif
                sendPackage(senderID, ACK, NULL);
            }
            break;
        case ACK:
            if(receiverID == localID){
                Serial.println(" --- Message LoRa receive ACK --- ");
                Serial.printf("Sender: 0x%2X |  Destination: 0x%2X  |  Message ID: ACK  |  Message: %s  |  RSSI:  %d  | SNR:  %.2f  \r\n", 
                                senderID, receiverID, incomingMsg, LoRa.packetRssi(), LoRa.packetSnr()); 
                recieveACK = true;
            }
            break;
        default:
            break;
    }
}

void sendDataToCloud(void){
    parseSensorData(strdup(incomingMsg.c_str()));
    if (WiFi.status() == WL_CONNECTED) {
        // JSON creation
        newJSON(incomingTempT, incomingHumidity);
        // Encryption data
        encryptData();
        // Post Function
        SendPost();
        delay(500);
    } else {
        Serial.println("");
        Serial.println("Failed to connect!");
        setupWiFi();
    }
}

void parseSensorData(const char *sensorData) {
    char* token;
    char dataCopy[strlen(sensorData) + 1];
    strcpy(dataCopy, sensorData);

    // Firts token (temperature Celsius)
    token = strtok(dataCopy, ";");
    if (token != NULL) {
        incomingTempT = atoi(token); // Convert token to int 
    } else {
        Serial.println(" --- Error: Could not get temperature\r\n");
        incomingTempT = 0;
        return;
    }
    // Second token (humidity)
    token = strtok(NULL, ";");
    if (token != NULL) {
        incomingHumidity = atof(token); // Convert token to int 
    } else {
        Serial.println(" --- Error: Could not get humidity\r\n");
        incomingHumidity = 0;
        return;
    }
}
