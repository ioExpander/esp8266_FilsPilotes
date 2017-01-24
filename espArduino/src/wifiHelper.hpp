#ifndef WIFIHELPER_h
#define WIFIHELPER_h

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

void wifiConnect() {
  //use to debug wifi issues
  //Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
      yield();
  }

  Serial.println("");
  Serial.print("WiFi connected to : ");
  Serial.print(WiFi.SSID());
  Serial.print(" Signal = ");
  Serial.println(WiFi.RSSI());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.flush();
  yield();
}

#endif //#ifndef WIFIHELPER_h
