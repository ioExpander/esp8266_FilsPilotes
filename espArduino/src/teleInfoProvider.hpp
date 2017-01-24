#ifndef TELEINFOPROVIDER_h
#define TELEINFOPROVIDER_h

#include <Time.h>

//Enable to allow debug output from this module !
//#define DEBUG_TELEINFOPROVIDER

#define TINFOFETCHINTERVAL 60 //Seconds between TInfo fetches ! => 120.

void fetchTInfoData();
void setupTInfoProvider();
long getLastIndexHC();
long getLastIndexHP();
time_t getLastTInfoFrameTime();

#endif //#ifndef TELEINFOPROVIDER_h
