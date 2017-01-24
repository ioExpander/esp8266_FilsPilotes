
#include "timeProvider.hpp"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <Wire.h> // must be included here so that Arduino library object file references work
//#include <TimeLib.h>
#include <DS3232RTC.h>


#ifdef DEBUG_TIMEPROVIDER
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINT(x)  Serial.print (x)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
#endif

WiFiUDP Udp;


// NTP Servers:
static const char ntpServerName[] = "0.fr.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";


const int timeZone = 1;     // Central European Time Winter
//const int timeZone = 2;     // Central European Time Summer
//const int timeZone = -5;  // Eastern Standard Time (USA)

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets
bool hasRtcOscStopped = false;
int ntpErrorCount = 0;
time_t nextNtpCheck = 0;
time_t lastNtpCheck = 0;
time_t bootTime = 0;
char formattedTimeBuffer[15];
char formattedDateBuffer[15];
char nextNtpCheckBuf[15];
char uptimeBuf[16];

char* formatTime(time_t time) {
  sprintf(formattedTimeBuffer, "%02u:%02u:%02u", hour(time), minute(time), second(time));
  return formattedTimeBuffer;
}

char* getFormattedDate(time_t time) {
  sprintf(formattedDateBuffer, "%02u/%02u/%04u", day(time), month(time), year(time));
  return formattedDateBuffer;
}

char* getFormattedEspTime() {
  return formatTime(now());
}

time_t getEspTime() {
  return now();
}

char* getFormattedEspDate() {
  return getFormattedDate(now());
};

bool getHasRtcOscStopped() {
  return hasRtcOscStopped;
}

char* getFormattedLastNtpUpdateTime() {
  return formatTime(lastNtpCheck);
}

char* getFormattedNextNtpUpdateTime() {
  return formatTime(nextNtpCheck);
}

char* getFormattedBootTime() {
  return formatTime(bootTime);
}

char* getFormattedUptime() {
  sprintf(uptimeBuf, "%d", now() - bootTime);
  return uptimeBuf;
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
  yield();
}

time_t getNtpTime()
{
  yield();
  IPAddress ntpServerIP; // NTP server's ip address
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  DEBUG_PRINTLN(F("Transmit NTP Request"));
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  DEBUG_PRINT(ntpServerName);
  DEBUG_PRINT(": ");
  DEBUG_PRINTLN(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      DEBUG_PRINTLN(F("Receive NTP Response"));
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      lastNtpCheck = secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;

      DEBUG_PRINT("Got Time from NTP : ");
      DEBUG_PRINTLN(formatTime(lastNtpCheck));
      nextNtpCheck = lastNtpCheck + COUNTSIFNTPSUCCESS * RTCSYNCINTERVAL;
      return lastNtpCheck;
    }
  }
  ntpErrorCount += 1;
  DEBUG_PRINT(F("Error - No NTP Response - #"));
  DEBUG_PRINTLN(ntpErrorCount);
  nextNtpCheck = lastNtpCheck + COUNTSIFNTPFAILURE * RTCSYNCINTERVAL;
  return 0; // return 0 if unable to get the time
}


time_t getRtcTime() {
  //Only update NTP Time once if RTC status is wrong
  if (RTC.oscStopped(false)) {
    // Common Cuases:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing
    DEBUG_PRINTLN("RTC lost confidence in the DateTime!");
    if (!hasRtcOscStopped) {
      DEBUG_PRINTLN("Updating Broken RTC using NTP");
      hasRtcOscStopped = true;
      time_t ntpTime = getNtpTime();
      if (ntpTime != 0) {
        RTC.set(ntpTime);
        RTC.oscStopped(true); //Clear osc stopped bit in RTC Registry
        yield();
        return RTC.get();
      } else {
        DEBUG_PRINTLN("Updating Broken RTC failed !");
        return 0;
      }
    }
  }
  //RTC Time is Valid
  if (RTC.get() >= nextNtpCheck ) {
    yield();
    //Time to update the RTC clock using NTP
    DEBUG_PRINTLN("Time to Update Working RTC Using NTP");
    time_t ntpTime = getNtpTime();
    if (ntpTime != 0) {
      RTC.set(ntpTime); yield();
    }
  } else {
    DEBUG_PRINT("Next NTP Update At : ");
    DEBUG_PRINTLN(formatTime(nextNtpCheck));
  }
  yield();
  DEBUG_PRINTLN("Got time from RTC");
  return RTC.get();
}


void setupRtcTime() {
  //RTC = DS3232RTC();    //instantiate an RTC object
  RTC.begin();
  yield();

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  RTC.squareWave(SQWAVE_NONE);
  yield();
}

void setupNtpTime() {
  //Start UDP Session
  Udp.begin(LOCALUDPPORT);
}

void setupTimeProvider() {
  setupNtpTime(); yield();
  setupRtcTime(); yield();

  setSyncProvider(getRtcTime);
  setSyncInterval(RTCSYNCINTERVAL);
  bootTime = now();
  //Initialize next NTP check Time
  nextNtpCheck = bootTime + RTCSYNCINTERVAL * COUNTSIFNTPFAILURE;
  DEBUG_PRINT("Time Provider Setup : ");
  DEBUG_PRINT(getFormattedEspDate());
  DEBUG_PRINT(" ");
  DEBUG_PRINTLN(getFormattedEspTime());
  DEBUG_PRINT("RTC: ");
  DEBUG_PRINTLN(formatTime(RTC.get()));
  DEBUG_PRINT("Next NTP check");
  DEBUG_PRINTLN(formatTime(nextNtpCheck));
  DEBUG_PRINTLN("--------------------");
  yield();
}
