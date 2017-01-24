#include "filpiloteProvider.hpp"
#include <Adafruit_MCP23017.h>

Adafruit_MCP23017 mcp;

int sortiesFP[NBFILSPILOTES][2] = { {FP1},{FP2},{FP3},{FP4},{FP5},{FP6}};
char etatFP[NBFILSPILOTES];

void initFilPilote() {
  mcp.begin();      // use default address 0
  for (int i=0; i < 16; i++) {
      mcp.pinMode(i, OUTPUT);
      mcp.digitalWrite(i, HIGH);
      yield();
  }
  for (int i = 0; i< NBFILSPILOTES; i++) {
    etatFP[i] = ECO_STATE;
  }
  yield();
}

String getFormattedFpValues() {
  String outputString = "";
  for (int i = 0; i< NBFILSPILOTES; i++) {
    outputString.concat(etatFP[i]);
  }
  return outputString;
}


bool setFP(int FPid, char command) {
  if (FPid >= NBFILSPILOTES) {
    return false;
  }
  if (command != CONFORT_STATE && command != HG_STATE && command != ARRET_STATE && command != ECO_STATE ) {
    return false;
  }

    if (command == CONFORT_STATE) {
      mcp.digitalWrite(sortiesFP[FPid][0], LOW);
      mcp.digitalWrite(sortiesFP[FPid][1], LOW);
    } else if (command == HG_STATE) {
      mcp.digitalWrite(sortiesFP[FPid][0], HIGH);
      mcp.digitalWrite(sortiesFP[FPid][1], LOW);
    } else if (command == ARRET_STATE) {
      mcp.digitalWrite(sortiesFP[FPid][0], LOW);
      mcp.digitalWrite(sortiesFP[FPid][1], HIGH);
    } else if (command == ECO_STATE) {
      mcp.digitalWrite(sortiesFP[FPid][0], HIGH);
      mcp.digitalWrite(sortiesFP[FPid][1], HIGH);
    }
    etatFP[FPid] = command;
    yield();
    return(true);
}
