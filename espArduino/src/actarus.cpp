/*
 *
 *
 */
#include "hardwareHelper.hpp"
#include "wifiHelper.hpp"
#include "otaHelper.hpp"
#include "timeProvider.hpp"
#include "crashReportProvider.hpp"
#include "debugProvider.hpp"
#include "filpiloteProvider.hpp"
#include "teleInfoProvider.hpp"
#include "webServing.hpp"


#include <ESP8266WiFiMulti.h>

#define HOSTNAME "Actarus"

void setup() {
    Serial.begin(115200);
    delay(100);

    setupHardware(); yield();
    //i2cSearch(); yield();
    //listSpiFS(); yield();

    // We start by connecting to a WiFi network
    WiFiMulti.addAP("Goldorak", "fibrefeebre");
    WiFiMulti.addAP("Chamalo", "fibrefeebre");
    wifiConnect();
    yield();

    otaSetup(HOSTNAME);
    yield();

    setupTimeProvider(); yield();
    Serial.print("Time Service started Current time is : ");
    Serial.println(getFormattedEspTime());

    setupRemoteWsDebugger(); yield();
    Serial.println("Debug webSocket server Started");

    //Init MCP23017 and FP provider
    initFilPilote();  yield();
    Serial.print("Fil Pilotes Activés : ");
    Serial.println(getFormattedFpValues());

    setupTInfoProvider(); yield();
    Serial.println("TInfo Provider Started");

    setupWebServer(); yield();
    Serial.println("API server Started");

    SaveCrash.print();
}


void loop() {
    handleRemoteDebugWsConnections(); yield();
    handleWebClient(); yield();
    fetchTInfoData();
    yield();
    ArduinoOTA.handle(); yield();
}
