#ifndef CRASHREPORTHELPER_h
#define CRASHREPORTHELPER_h

#include <WString.h>
#include <EspSaveCrash.h>


void generateCrash(char* crashName);
void clearCrashes();

String getFormattedCrashCount();


#endif //#ifndef CRASHREPORTHELPER_h
