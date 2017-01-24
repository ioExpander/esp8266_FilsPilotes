#ifndef WEBSERVING_h
#define WEBSERVING_h

#include <ESP8266WebServer.h>

//Uncomment ton enable Debug in this module !
//#define DEBUG_WEBSERVING


String getContentType(String filename);
bool handleFileRead(String path);
bool handleRoot();
void handleNotFound();
void setupWebServer();
void handleWebClient();


#endif //#ifndef WEBSERVING_h
