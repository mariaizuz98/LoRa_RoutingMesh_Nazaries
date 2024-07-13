#include "routing.h"

extern byte localID;

routeTableEntry routeTable;

void setupRoutingTable(void){
    Serial.print("*** Creating Route Table...");
    routeTable.destinationAddress = GATEWAY_ID;
    routeTable.nextHop = 0;
    routeTable.hopCount = 0;
    strcpy(routeTable.sequenceRoute, " ");

    Serial.printf(" Route Table(dest,nextHop,countHop,sequenceRoute):(  %d  |  %d  |  %d  |  %s  )\r\n", 
                    routeTable.destinationAddress, routeTable.nextHop, routeTable.hopCount, routeTable.sequenceRoute);
}

void sendRREQ(byte destinationId){

    char sequenceRREQ[50];
    sprintf(sequenceRREQ,"%d;",localID);
    strcpy(routeTable.sequenceRoute, sequenceRREQ);
    sendPackage(destinationId, RREQ, routeTable.sequenceRoute);

}

void sendRREP(byte destinationId, char* incomingSequence){

    char sequenceRREP[50];
    sprintf(sequenceRREP,"%s%d;", incomingSequence, localID);
    strcpy(routeTable.sequenceRoute,  sequenceRREP);
    sendPackage(destinationId,RREP,routeTable.sequenceRoute);

}

void updateRouteTable(const char *sequenceRoute){

    byte byte_nextHop;

    Serial.print(" *** Updating Route Table...");
    char* char_nextHop = getNextHop(sequenceRoute);
    //sscanf(char_nextHop,"%hhx", &byte_nextHop);

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

char* getNextHop(const char *sequenceRoute) {
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
