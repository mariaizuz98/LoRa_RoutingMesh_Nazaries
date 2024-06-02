#include "http.h"

/******** Parameter definitions ********/
// Se crea la instancia rtc en ESP32Time
ESP32Time rtc;
// Se crea la instancia para encryption and hashing
mbedtls_md_context_t mdctx;
mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

/******** Global Vars Definitions ********/
String stringJSON;
char json[200];
char Signature[0];
bool res;
float temp;
float hum;

/******** Station Definition ********/
String createdDate = "05.04.2024";                // Fecha de creación de la estación
String version = "V2.0";                          // Versión del dispositivo
String MIZ[] = { "MIZ001", "MIZ002", "MIZ003" };  // IMEI o serial number de la estación
int i;                                            // Selector de nodo. Este lo puedes poner como tú quieras

/******** NTP Parameters Definition ********/
const char *ntpServer = "es.pool.ntp.org";  // NTP server to request epoch time
const long gmtOffset_sec = -5 * 3600;
const int daylightOffset_sec = 0;

/******** Server Name Definition ********/
// Your Domain name with URL path or IP address with path
String serverName = "https://ceres-api-tp1.nazaries.cloud/third_party_api/v1/submit/data";

/******** Security. API KEY ********/
String APIkey = "f6341e4d2ed05a18f02c8458e05e2b6d";              // MIZ API-KEY
const char *APIkey_secret = "cd9d7cc64b6b35ffc98e7f38a29f4f63";  // MIZ API-KEY secret

extern byte senderID;          // sender address

/***************  Timestamp initialisation ***************/
void updateTime(void) {  // Function that gets current epoch time from a NTP Server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.setTimeStruct(timeinfo);
  }
}

/*************** JSON creation ***************/
void newJSON(int t, int h) {
  StaticJsonDocument<200> doc;

  // Nested objects
  JsonObject obj = doc.createNestedObject("HIST");
  if(senderID == 0x70){
    i = 0;
  } else if (senderID == 0xE8){
    i = 1;
  } else if (senderID == 0xF4){
    i = 2;
  }
  // Header
  obj["VERSION"] = version;
  obj["IMEI"] = MIZ[i];
  obj["UTC"] = rtc.getEpoch();

  JsonObject dat = obj.createNestedObject("DATA");
  dat["TA_0"] = t;
  dat["HR_0"] = h;

  // Conversión to json
  serializeJson(doc, json);
  serializeJson(doc, stringJSON);

  // Display json in pretty mode
  serializeJsonPretty(doc, Serial);
}


/*************** Encryption Function ***************/
void encryptData(void) {

  // Confección del mensaje a encriptar
  char data[200];                                               // mensaje HTTP
  int index = serverName.indexOf("//");                         // Encontrar la posición de "//" en el serverName
  String url = "POST" + serverName.substring(index + 2) + "?";  // 1a parte del mensaje HTTP

  // Concatenar el mensaje
  url.toCharArray(data, 200);  // 1a parte del mensaje HTTP. Url del servidor
  strcat(data, json);          // 2a parte del mensaje HTTP. Cuerpo del mensaje
  Serial.println("");
  Serial.print("mensaje a encriptar: ");
  Serial.println(data);

  // calculate size of the key and data
  const size_t keyLength = strlen(APIkey_secret);
  const size_t dataLength = strlen(data);

  // init hash function
  mbedtls_md_init(&mdctx);
  mbedtls_md_setup(&mdctx, mbedtls_md_info_from_type(md_type), 1);                  // setup with the prespecified hash (md_type)
  mbedtls_md_hmac_starts(&mdctx, (const unsigned char *)APIkey_secret, keyLength);  // start the hash with the Secret key

  // update hash
  mbedtls_md_hmac_update(&mdctx, (const unsigned char *)data, dataLength);

  // finalize hash
  byte output_HMAC[32];  // byte datatype is needed to read the output_hmac correct
  unsigned char output[65];
  size_t outlen;

  // return the final HMAC number
  mbedtls_md_hmac_finish(&mdctx, output_HMAC);
  mbedtls_md_free(&mdctx);

  mbedtls_base64_encode(output, 64, &outlen, (unsigned char *)output_HMAC, sizeof(output_HMAC));

  // null-terminate the output string
  output[outlen] = 0;
  // Serial.print((const char *)output);

  sprintf(Signature, "%s", (const char *)output);
  Serial.print("Signature: ");
  Serial.println(Signature);
}


/*************** Post Function ***************/
void SendPost(void) {
  HTTPClient http;

  // Specify the url
  http.begin(serverName);
  // Specify content-type header
  http.addHeader("content-Type", "application/json");
  http.addHeader("X-API-Key", APIkey);
  http.addHeader("X-Signature", Signature);

  int httpCode;
  unsigned long timer;

  // Send a message a maximum of 5 times if a code 200 is not answered.
  for (int i = 0; i < 5; i++) {
    // Reset the timer
    timer = millis();
    // Send the actual Post request
    httpCode = http.POST(stringJSON);

    // If the response is a code 200, do a loopback for.
    if (httpCode == 200) {
      i = 5;
      // Else, wait a delay of a minute for each iteration
    } else {
      while (millis() - timer > 60000) {}
    }
  }

  // Check for the returning code
  if (httpCode > 0) {
    // Get the response to the request
    String response = http.getString();

    // Print return code
    Serial.println("");
    Serial.print("Code: ");
    Serial.println(httpCode);
    // Print request answer
    Serial.println(response);

  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpCode);
  }

  // Free resources
  http.end();
}
