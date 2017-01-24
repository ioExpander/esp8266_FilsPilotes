#include "debugProvider.hpp"
#include "timeProvider.hpp"
#include <WebSocketsServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);
bool isDebugClientConnected = false;
uint8_t debugClientId = 0;
char remoteDebugBuff[300];
bool isDebugLineInProgress = false;

#ifdef DEBUG_DEBUGPROVIDER
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINT(x)  Serial.print (x)
 #define DEBUG_PRINTF(...) Serial.printf (__VA_ARGS__)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
 #define DEBUG_PRINTF(...)
#endif


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            {
              DEBUG_PRINTF("[%u] Disconnected!\n", num);
              if (isDebugClientConnected && num == debugClientId) {
                isDebugClientConnected = false;
                DEBUG_PRINTLN(F("Remote Debug Client Disconnected"));
              }
            }
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                DEBUG_PRINTF("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                if (strcmp((const char*)payload,"/debug") == 0) {
                  debugClientId = num;
                  isDebugClientConnected = true;
                  DEBUG_PRINTLN(F("Remote Debug Client Connected !"));
                  // send message to client
                  webSocket.sendTXT(num, "Hello Remote Debug User !\n");
                }
                else {
                  //some Other URL
                }
            }
            break;
        case WStype_TEXT:
            DEBUG_PRINTF("[%u] get Text: %s\n", num, payload);

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            DEBUG_PRINTF("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);

            // send message to client
            // webSocket.sendBIN(num, payload, lenght);
            break;
    }
    yield();
}

void debugWrite(const String msg, bool isNewLine) {
  //Build Debug line.
  String debugLine = "";
  //Add timestamp if needed !
  if (!isDebugLineInProgress) { //Begin line so add timestamp
    debugLine = "[";
    debugLine += getFormattedEspTime();
    debugLine += "]";
    isDebugLineInProgress = true;
  }
  //add msg payload
  debugLine += msg;
  //add new line marker if needed
  if (isNewLine) {
    debugLine += "\n";
    isDebugLineInProgress = false;
  }

  //Send formatted debug line
  if (isDebugClientConnected){
    //webSocket.sendTXT(debugClientId, "[T8TO]");
    debugLine.toCharArray(remoteDebugBuff, 300);
    webSocket.sendTXT(debugClientId, remoteDebugBuff);
  }
  Serial.print(debugLine);
  Serial.flush();
}

void debugWrite(const String msg) {
  debugWrite(msg, false);
  yield();
}

void debugWriteln(const String msg) {
  debugWrite(msg, true);
  yield();
}

void setupRemoteWsDebugger() {
  DEBUG_PRINTLN("Starting Debug Socket Server");
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  yield();
};

void handleRemoteDebugWsConnections() {
    webSocket.loop();
    yield();
};
