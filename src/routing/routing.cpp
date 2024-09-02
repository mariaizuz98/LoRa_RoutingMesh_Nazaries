#include "routing.h"

#define HOPS_INI    0
#define MAX_RREPS 10
#define STORAGE_TIME 5000 // Tiempo en milisegundos para almacenar RREPs

struct RREP_received {
    int rssi;
    char sequenceRoute[50];
    int hopCount;
    unsigned long timestamp;
};

RREP_received rrepStorage[MAX_RREPS];
int rrepCount = 0;
unsigned long lastUpdateTime = 0;

extern byte localID;

routeTableEntry routeTable;

void setupRoutingTable(void){
    char sequenceRREQ[50];

    Serial.print(" *** Creating Route Table...");
    routeTable.destinationAddress = GATEWAY_ID;
    routeTable.nextHop = 0;
    routeTable.hopCount = HOPS_INI;
    sprintf(sequenceRREQ,"%d;",localID);
    strcpy(routeTable.sequenceRoute, sequenceRREQ);

    Serial.printf(" Route Table(dest,nextHop,countHop,sequenceRoute):(  %d  |  %d  |  %d  |  %s  )\r\n", 
                    routeTable.destinationAddress, routeTable.nextHop, routeTable.hopCount, routeTable.sequenceRoute);
}

void resetRoutingTable(void){
    char sequenceRREQ[50];

    Serial.print(" *** Reset Route Table...");
    routeTable.destinationAddress = GATEWAY_ID;
    routeTable.nextHop = 0;
    routeTable.hopCount = HOPS_INI;
    sprintf(sequenceRREQ,"%d;",localID);
    strcpy(routeTable.sequenceRoute, sequenceRREQ);

    Serial.printf(" Route Table(dest,nextHop,countHop,sequenceRoute):(  %d  |  %d  |  %d  |  %s  )\r\n", 
                    routeTable.destinationAddress, routeTable.nextHop, routeTable.hopCount, routeTable.sequenceRoute);
}

void sendRREQ(byte destinationId){

    // char sequenceRREQ[50];
    // sprintf(sequenceRREQ,"%d;",localID);
    // strcpy(routeTable.sequenceRoute, sequenceRREQ);
    sendPackage(localID, localID, destinationId, RREQ, routeTable.sequenceRoute);

}

void sendRREP(byte destinationId, const char* incomingSequence){

    char sequenceRREP[50];
    // #ifdef NODE_LORA
    //     if(strcmp(routeTable.sequenceRoute, " ") == 0){
    //         sprintf(sequenceRREP,"%s%d;", incomingSequence, localID);
    //         strcpy(routeTable.sequenceRoute, sequenceRREP);
    //     }
    //     sendPackage(destinationId,RREP,routeTable.sequenceRoute);
    // #else
        sprintf(sequenceRREP,"%s%s", incomingSequence, routeTable.sequenceRoute);
        sendPackage(localID, localID, destinationId, RREP, sequenceRREP);
    // #endif

}

void sendRRER(byte destinationId){
    representLCD_LostConection();
    sendPackage(localID, localID, destinationId, RERR, NULL);
    resetRoutingTable();
}

void analyzeSeqRoute(int rssi, const char *sequenceRoute) {
    if (rssi > -120) {
        int totalHopCount = getTotalHopToGateway(sequenceRoute);
        // Almacenar el RREP recibido
        if (rrepCount < MAX_RREPS){
            if(totalHopCount != 0){
                Serial.printf(" * New RREP %d\r\n", rrepCount);
                rrepStorage[rrepCount].rssi = rssi;
                strcpy(rrepStorage[rrepCount].sequenceRoute, sequenceRoute);
                rrepStorage[rrepCount].hopCount = totalHopCount;
                rrepStorage[rrepCount].timestamp = millis();
                rrepCount++;
            }
        } else {
            // Manejo en caso de desbordamiento, aquí simplemente descartamos el nuevo RREP
            Serial.println("Warning: RREP storage full, discard RREP");
        }
        if(totalHopCount < routeTable.hopCount){
            // sprintf(newSequenceRoute,"%d;%s", localID, sequenceRoute);
            // updateRouteTable(newSequenceRoute);
            updateRouteTable(sequenceRoute);
        }
    }
    // Revisar si ha pasado suficiente tiempo para evaluar las rutas almacenadas
    if (millis() - lastUpdateTime > STORAGE_TIME) {
        evaluateAndSelectBestRoute();
        lastUpdateTime = millis();
    }
}

void evaluateAndSelectBestRoute(void) {
    if (rrepCount == 0) {
        Serial.println("Warning: no routes to evaluate");
        return; // No hay rutas almacenadas para evaluar
    }

    RREP_received bestRoute = rrepStorage[0];
    for (int i = 1; i < rrepCount; i++) {
        // Comparar por número de saltos primero
        if (rrepStorage[i].hopCount < bestRoute.hopCount) {
            bestRoute = rrepStorage[i];
        } else if (rrepStorage[i].hopCount == bestRoute.hopCount) {
            // Si el número de saltos es igual, comparar por RSSI
            if (rrepStorage[i].rssi > bestRoute.rssi) {
                bestRoute = rrepStorage[i];
            }
        }
    }
    // Actualizar la tabla de rutas con la mejor ruta encontrada
    updateRouteTable(bestRoute.sequenceRoute);
    // Limpiar el almacenamiento de RREPs
    rrepCount = 0;
}


int getTotalHopToGateway(const char *sequenceRoute){
    char* token;
    char* nextHop = NULL;  
    char sequenceCopy[strlen(sequenceRoute) + 1];
    int hopCount = 0;  
    const int targetValue = GATEWAY_ID;  
    bool foundGateway = false;
    strcpy(sequenceCopy, sequenceRoute);
    // Extracción del primer token
    token = strtok(sequenceCopy, ";");
    while (token != NULL) {
        hopCount++; 
        int tokenValue = atoi(token);
        if (tokenValue == targetValue) {
            nextHop = (char*) malloc(strlen(token) + 1);
            if (nextHop == NULL) {
                Serial.printf("Error: Could not allocate memory for nextHop\n");
                break;
            }
            strcpy(nextHop, token);  // Copia segura del token
            foundGateway = true;
            break;  // Salir del bucle ya que se encontró el valor objetivo
        }
        token = strtok(NULL, ";");  // Obtener el siguiente token
    }
    // Si no se encuentra el gateway, descarta la secuencia
    if(!foundGateway){
        hopCount = 0;
    }
    return hopCount;  // Número de saltos (tokens analizados menos uno)
}

void updateRouteTable(const char *sequenceRoute){
    byte byte_nextHop;
    Serial.print(" *** Updating Route Table...");
    char* char_nextHop = getNextHop(sequenceRoute);
    //sscanf(char_nextHop,"%hhx", &byte_nextHop);
    // Verificar si char_nextHop no es nulo
    if (char_nextHop == NULL) {
        Serial.println("Error: getNextHop returned NULL.");
        return;
    }
    if (sscanf(char_nextHop, "%d", &byte_nextHop) != 1) {
        Serial.println("Error: Failed to parse next hop.");
        return;
    }
    routeTable.nextHop = byte_nextHop;
    strcpy(routeTable.sequenceRoute, sequenceRoute);
    Serial.printf(" Route Table(dest,nextHop,countHop,sequenceRoute):(  %d  |  %d  |  %d  |  %s  )\r\n", 
                    routeTable.destinationAddress, routeTable.nextHop, routeTable.hopCount, routeTable.sequenceRoute);
    representLCD_Node();
}

char* getNextHop(const char *sequenceRoute){
    char* token;
    char* secondToken = NULL;  // Declaración como NULL
    char sequenceCopy[strlen(sequenceRoute) + 1];
    int tokenCount = 0;  // Para contar la cantidad de tokens
    strcpy(sequenceCopy, sequenceRoute);
    // Extracción del primer token (no utilizado directamente aquí)
    token = strtok(sequenceCopy, ";");
    while (token != NULL) {
        tokenCount++; // Incrementa el contador de tokens
        if (tokenCount == 2) {
            secondToken = (char*) malloc(strlen(token) + 1);  // Asignación de memoria para secondToken
            if (secondToken == NULL) {
                Serial.printf("Error: No se pudo asignar memoria para secondToken\n");
                return NULL;  // Retorno seguro en caso de falla de memoria
            }
            strcpy(secondToken, token);  // Copia segura del token
        }
        token = strtok(NULL, ";");  // Obtener el siguiente token
    }
    routeTable.hopCount = tokenCount - 1;  // Ajuste si es necesario, dependiendo de cómo se usa hopCount
    return secondToken;  // Devuelve el segundo token, o NULL si no fue encontrado
}
