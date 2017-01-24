#ifndef TIMEPROVIDER_h
#define TIMEPROVIDER_h

#include <Time.h>

//Uncomment ton enable Debug in this module !
//#define DEBUG_TIMEPROVIDER

#define RTCSYNCINTERVAL 120 //seconds before Update using RTC. => 60
#define COUNTSIFNTPSUCCESS 30 //times RTCSYNCINTERVAL => 30
#define COUNTSIFNTPFAILURE 5 //times RTCSYNC5NTERVAL => 5

#define NTP_PACKET_SIZE 48 // NTP time is in the first 48 bytes of message
#define LOCALUDPPORT 8888 // local port to listen for UDP packets


//Init Ntp service and RTC service
void setupTimeProvider();

//void printTimeToSerial();

time_t getEspTime();
char* getFormattedEspTime();
char* getFormattedEspDate();
bool  getHasRtcOscStopped();
char* getFormattedLastNtpUpdateTime();
char* getFormattedNextNtpUpdateTime();
char* getFormattedUptime();
char* getFormattedBootTime();

char* formatTime(time_t time);

#endif //ifndef TIMEPROVIDER_h
