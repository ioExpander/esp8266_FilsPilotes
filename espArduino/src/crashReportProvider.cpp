#include "crashReportProvider.hpp"

#include <EspSaveCrash.h>


void generateCrash(char* crashName) {
  if (strcmp(crashName,"nullPtr") == 0) {
    Serial.println("Called Crash nullPtr");
    //@Todo generateNUllPointer...
  }
  if (strcmp(crashName,"divBy0") == 0) {
    Serial.println("Called Crash divBy0");
    int toto = 0;
    Serial.println(10/toto);
  }
}

String getFormattedCrashCount() {
    return String(SaveCrash.count());
}

void clearCrashes() {
  SaveCrash.clear();
}
