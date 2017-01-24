#ifndef HARDWAREHELPER_h
#define HARDWAREHELPER_h

#include <Wire.h>
#include <FS.h>


ADC_MODE(ADC_VCC); // to use getVcc

void setupHardware() {
  //Start SPI FS file system
  SPIFFS.begin();
  yield();
}


void listSpiFS() {
  Dir dir = SPIFFS.openDir("/");
  Serial.println("Listing Flash Contents");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
  }
  Serial.printf("\n");
  Serial.flush();
  yield();
}


void i2cSearch() {
  Serial.println(F("Searching for I2C devices"));
  byte count = 0;
  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    yield();
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
      {
      Serial.print (F("Found i2c Device Address: "));
      Serial.print (i, DEC);
      Serial.print (F(" (0x"));
      Serial.print (i, HEX);
      Serial.println (F(")"));
      count++;
      } // end of good response
      delay (10);
  } // end of for loop
  Serial.println (F("Done."));
  Serial.print (F("Found "));
  Serial.print (count, DEC);
  Serial.println (F(" device(s)."));
  Serial.flush();
  yield();
}

#endif //#ifndef HARDWAREHELPER_h
