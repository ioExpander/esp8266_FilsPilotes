// **********************************************************************************
// Some Serial & Remote Debugging routines
// **********************************************************************************
// Copyright (C) 2016 Aditye Kumar
// Licence MIT
//
//
// @TODO : Document functions !
//
//
// **********************************************************************************
#ifndef DEBUGPROVIDER_h
#define DEBUGPROVIDER_h

//#include <TimeLib.h>
#include <WString.h>

//Uncomment to enable Debug msg in this lib.
//#define DEBUG_DEBUGPROVIDER


void setupRemoteWsDebugger();
void debugWrite(const String msg); //Will output msg to Serial and/or Websocket connection
void debugWriteln(const String msg); //Will output msg to Serial and/or Websocket connection
void handleRemoteDebugWsConnections();






#endif //ifndef TIMEPROVIDER_h
