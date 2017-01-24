// **********************************************************************************
// Some Functions to control the Fil Pilote Output
// **********************************************************************************
// Copyright (C) 2016 Aditye Kumar
// Licence MIT
//
// Inspired by some great work from Charles-Henri Hallard (http://hallard.me) and
// Thibault Ducret
//
// @TODO : Document functions !
//
//
// **********************************************************************************
#ifndef FILPILOTEPROVIDER_h
#define FILPILOTEPROVIDER_h

#include <Arduino.h>
#include <Adafruit_MCP23017.h>

#define NBFILSPILOTES 6
#define FP1 0,1  //Salon
#define FP2 2,3  //SdB
#define FP3 4,5  //ChPa
#define FP4 6,7  //ChBB
#define FP5 8,9  //Bur
#define FP6 10,11 //Coul+Cuisine

extern Adafruit_MCP23017 mcp;

void initFilPilote();
bool setFP(int FPid, char command);
String getFormattedFpValues();


#define CONFORT_STATE 'C' //Pas de signal
#define HG_STATE 'H'  //Hors Gel = demi alternance négative
#define ARRET_STATE 'A' //Arret = demi alternance potisive
#define ECO_STATE 'E' //Oscillation complète


#endif //ifndef FILPILOTEPROVIDER_h
