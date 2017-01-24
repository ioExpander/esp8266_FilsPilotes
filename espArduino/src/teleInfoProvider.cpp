#include "teleInfoProvider.hpp"
#include <SoftwareSerial.h>
#include <LibTeleinfo.h>
#include <Ticker.h>
#include "timeProvider.hpp"
#include "debugProvider.hpp"

#ifdef DEBUG_TELEINFOPROVIDER
 #define DEBUG_PRINTLN(x)  debugWriteln (x)
 #define DEBUG_PRINT(x)  debugWrite (x)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
#endif


TInfo          tinfo; // Teleinfo object
SoftwareSerial * swSer;
bool isFetchingTInfo = false;
bool shouldFetchTInfo = false;

Ticker tickerTInfoStart;
time_t lastTInfoFrameTime = 0;
long lastIndexHC = 0;
long lastIndexHP = 0;


long getLastIndexHC() {return lastIndexHC;}
long getLastIndexHP() {return lastIndexHP;}
time_t getLastTInfoFrameTime() {return lastTInfoFrameTime;}
//Create Class containing the indexes.

void endTInfoMeasure() {
  swSer->end();
  delete swSer;
  isFetchingTInfo = false;
  DEBUG_PRINTLN("=Ending TInfo Measure=");
}

void startTInfoMeasure(){
  if (!isFetchingTInfo) {
    DEBUG_PRINTLN("Starting TInfo Measure");
    isFetchingTInfo = true;
    tinfo.init();
    swSer = new SoftwareSerial(D7, D8, false, 256);
    swSer->begin(1200);
    yield();
  } else {
    DEBUG_PRINTLN("TInfo already running");
  }
}

/*@TODO : remove - unused !
void DataCallback(ValueList * me, uint8_t  flags)
{
  if (flags & TINFO_FLAGS_ADDED)
    debugWrite(F("NEW -> "));
  if (flags & TINFO_FLAGS_UPDATED)
    debugWrite(F("MAJ -> "));
  // Display values
  DEBUG_PRINT(x)(me->name);
  DEBUG_PRINT(x)("=");
  DEBUG_PRINTLN(x)(me->value);
}
*/


void NewFrameCallback(ValueList * me) {
  DEBUG_PRINTLN("Got New Frame !");
  endTInfoMeasure();
  lastTInfoFrameTime = getEspTime();
  yield();
  if (me) {
    //First value in list is always empty... so skip it
    //and last list flag is always MOTDETAT and is useless
    //so this stops just before the last item
    if (me->next) me = me->next; //skip first value
    while(me->next) {
      DEBUG_PRINT("Flag : ");
      if (me->name && me->value) {
        if (strcmp(me->name,"HCHC") == 0){
            lastIndexHC = atol(me->value);
        }
        if (strcmp(me->name,"HCHP") == 0){
            lastIndexHP = atol(me->value);
        }
        DEBUG_PRINT(me->name);
        DEBUG_PRINT(" = ");
        DEBUG_PRINTLN(me->value);
      }
      me = me->next;
      yield();
    }
    DEBUG_PRINTLN(String(lastIndexHC));
    DEBUG_PRINTLN(String(lastIndexHP));
  }
}

void tickTInfoStart() {
  shouldFetchTInfo = true;
}

void fetchTInfoData() {
  if (shouldFetchTInfo) {
    shouldFetchTInfo = false;
    if (isFetchingTInfo) {
      DEBUG_PRINTLN("Strange no Frame in a long time");
      endTInfoMeasure();
    } else {
      startTInfoMeasure();
    }
  }
  while (isFetchingTInfo && swSer && swSer->available() > 0) {
    tinfo.process(swSer->read());
    yield();
  }
}

void setupTInfoProvider() {
  tinfo.init();
  tinfo.attachNewFrame(NewFrameCallback);
  tickerTInfoStart.attach(TINFOFETCHINTERVAL, tickTInfoStart);
  tickTInfoStart(); //To start TInfo on boot
  yield();
}
