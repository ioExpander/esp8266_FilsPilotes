#include "webServing.hpp"
#include "timeProvider.hpp"
#include "filpiloteProvider.hpp"
#include "teleInfoProvider.hpp"
#include "debugProvider.hpp"
#include "crashReportProvider.hpp"
#include <ESP8266WebServer.h>
#include <FS.h>


#ifdef DEBUG_WEBSERVING
 #define DEBUG_PRINTLN(x)  debugWriteln (x)
 #define DEBUG_PRINT(x)  debugWrite (x)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
#endif

ESP8266WebServer * server;

String getContentType(String filename){
  if(server->hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  DEBUG_PRINTLN("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server->streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

bool handleRoot() {
  return handleFileRead("/toto.txt");
  yield();
  //server->send(200, "text/plain", "hello from GargaFilPilote!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += (server->method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";
  for (uint8_t i=0; i<server->args(); i++){
    message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
  }
  yield();
  server->send(404, "text/plain", message);
}

void handleEspStatus() {
  DEBUG_PRINTLN(F("API status called"));
  String message = "{";
  message += "\"EspTime\":\"";
  message += getFormattedEspTime();
  message += "\", \"date\":\"";
  message += getFormattedEspDate();
  message += "\", \"lastNtpUpdate\":\"";
  message += getFormattedLastNtpUpdateTime();
  message += "\", \"NextNtpUpdate\":\"";
  message += getFormattedNextNtpUpdateTime();
  message += "\", \"hasRtcOscStopped\":\"";
  if (getHasRtcOscStopped()) {
    message += "True !";
  } else message += "False";
  message += "\", \"BootTime\":\"";
  message += getFormattedBootTime();
  message += "\", \"BootDate\":\"";
  message += getFormattedBootDate();
  message += "\", \"Uptime\":\"";
  message += getFormattedUptime();
  message += "\", \"#Crashes\":\"";
  message += getFormattedCrashCount();
  message += "\", \"SSID\":\"";
  message += WiFi.SSID();
  message += "\", \"RSSID\":\"";
  message += WiFi.RSSI();
  yield();
  message += "\", \"Vcc\":\"";
  message += ESP.getVcc();
  message += "\", \"CpuFreqMHz\":\"";
  message += ESP.getCpuFreqMHz();
  message += "\", \"FlashChipReal\":\"";
  message += ESP.getFlashChipRealSize();
  message += "\", \"FlashChipSize\":\"";
  message += ESP.getFlashChipSize();
  message += "\", \"FreeHeap\":\"";
  message += ESP.getFreeHeap();
  message += "\", \"SketchSize\":\"";
  message += ESP.getSketchSize();
  message += "\", \"FreeSketchSpace\":\"";
  message += ESP.getFreeSketchSpace();
  message += "\"";
  message += "}";
  yield();
  server->send(200, "application/json", message);
  yield();
}

void handeTeleInfoStatus() {
  DEBUG_PRINTLN(F("API TeleInfo Status called"));
  String message = "{";
  //
  message += "\"IndexHP\":\"";
  message += getLastIndexHP();
  message += "\", \"IndexHC\":\"";
  message += getLastIndexHC();
  message += "\", \"LastTInfoFrameTime\":\"";
  message += formatTime(getLastTInfoFrameTime());
  message += "\"";
  //
  message += "}";
  yield();
  server->send(200, "application/json", message);
  yield();
}

void handleFpStatus() {
  DEBUG_PRINTLN(F("API FP status called"));
  String message = "{\"time\":\"";
  message += getFormattedEspTime();
  message += "\", \"date\":\"";
  message += getFormattedEspDate();
  message += "\", \"FpValues\":\"";
  message += getFormattedFpValues();
  message += "\"}";
  server->send(200, "application/json", message);
  yield();
}


void handleSetfp() {
  bool isArgsOK = false;
  int fpDisplayId = 0;
  char c_cmd = '0';
  if (server->hasArg("fpId") && server->hasArg("cmd")) {
      String s_fpId = server->arg("fpId");
      String s_cmd = server->arg("cmd");
      fpDisplayId = s_fpId.toInt();
      if (fpDisplayId > 0 && s_cmd.length() == 1) {
          c_cmd = s_cmd.charAt(0);

          isArgsOK = true;
      }
  }
  yield();
  if (isArgsOK) {
    DEBUG_PRINT(F("API setfp called "));
    DEBUG_PRINT(String(fpDisplayId));
    DEBUG_PRINTLN(String(c_cmd));

    if (setFP(fpDisplayId-1, c_cmd)) {
      String message = "{\"fpId\":\""+String(fpDisplayId);
      message += "\", \"setCmd\":\""+String(c_cmd);
      message += "\"}";
      server->send(200, "application/json", message);
    } else {
      String message = "Set FP Error !\n\n";
      message += "URI: ";
      message += server->uri();
      message += "\nMethod: ";
      message += (server->method() == HTTP_GET)?"GET":"POST";
      message += "\nArguments: ";
      message += server->args();
      message += "\n";
      for (uint8_t i=0; i<server->args(); i++){
        message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
      }
      server->send(501, "text/plain", message);
    }

  } else { //Bad args !
    String message = "Bad Args !\n\n";
    message += "URI: ";
    message += server->uri();
    message += "\nMethod: ";
    message += (server->method() == HTTP_GET)?"GET":"POST";
    message += "\nArguments: ";
    message += server->args();
    message += "\n";
    for (uint8_t i=0; i<server->args(); i++){
      message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
    }
    server->send(400, "text/plain", message);
  }
  yield();
}

void handleGenerateCrash() {
  if (server->hasArg("crashType")) {
    char tempCrashTypeBuffer[15];
    server->arg("crashType").toCharArray(tempCrashTypeBuffer,15);
    generateCrash(tempCrashTypeBuffer);
    server->send(200, "text/plain", F("Crashed"));
  } else {
    server->send(200, "text/plain", F("No Crash found !"));
  }
}

void handleGetCrashInfo() {
  WiFiClient client = server->client();
  if (client) {
    DEBUG_PRINTLN("\n[Crash Info Client connected]");
    client.print("HTTP/1.1 200 OK\r\n");
    client.print("Content-Type: text/plain\r\n");
    client.print("Connection: close\r\n");
    client.print("Access-Control-Allow-Origin: *\r\n");
    client.print("\r\n");
    yield();
    // send crash information to the web browser
    SaveCrash.print(client);
    yield();
  }
  delay(10); // give the web browser time to receive the data
  yield();
  // close the connection:
  client.stop(); yield();
}


void setupWebServer() {
  server = new ESP8266WebServer(80);
  server->on("/", HTTP_GET, [](){
    if(!handleRoot()) server->send(404, "text/plain", F("FileNotFound"));
  });
  server->on("/inline", [](){
    server->send(200, "text/plain", F("this works as well"));
  });
  server->on("/api/generateCrash", handleGenerateCrash);
  server->on("/api/clearCrashes", [](){
    clearCrashes();
    server->send(200, "text/plain", F("Crashes memory cleared !"));
  });
  server->on("/api/getCrashInfo", handleGetCrashInfo);
  server->on("/api/espStatus", handleEspStatus);
  server->on("/api/fpStatus", handleFpStatus);
  server->on("/api/setfp", handleSetfp);
  server->on("/api/teleInfoStatus", handeTeleInfoStatus);
  server->onNotFound(handleNotFound);

  server->begin();
  DEBUG_PRINTLN(F("HTTP server started"));
  yield();
}

void handleWebClient() {
  server->handleClient();
  yield();
}
