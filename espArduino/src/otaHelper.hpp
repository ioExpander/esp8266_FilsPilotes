#ifndef OTAHELPER_h
#define OTAHELPER_h

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>


void otaSetup(const char *hostname) {
  //Setup OTA
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
  ArduinoOTA.setHostname(hostname);

  ArduinoOTA.onStart([]() {
    Serial.println(F("\nOTA Start"));
  });
  ArduinoOTA.onEnd([]() {
    Serial.println(F("\nOTA End"));
    yield();
    ESP.restart();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.print(F("Progress: "));
    Serial.printf("%u%%\r", (progress / (total / 100)));
    yield();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("\nOTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
    Serial.flush();
    yield();
  });
  ArduinoOTA.begin();
  Serial.println(F("\nOTA Server Started"));
  Serial.flush();
  yield();
}

#endif //#ifndef OTAHELPER_h
