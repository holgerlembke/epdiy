/*
  Moin.

  If you made changes within the first examples "Aepdiy.ino" please copy the changes to this example.

  do {
    In this example we will display a simple 7-segement clock and do sleep.
    Additional, we only log into WiFi once a day to save energy.
  } while (1);  

  ReadTheDocs: https://epdiy.readthedocs.io/en/latest/api.html#highlevel-api
*/

// the wifi connection
#include <WiFi.h>
#include <WiFiMulti.h>
#include <esp_sntp.h>

// we want to do https:
#include <NetworkClientSecure.h>
#include <HTTPClient.h>

// a concept to keep your credentials out of your sketches
// you can remove this and add your ssid/passwort into IWiFi.ino
#include <privatedata.h>  // https://github.com/holgerlembke/privatedata

#include <epdiy.h>

// <<<Configuration>>>
const bool sleepinbetween = false;  // set to sleep, saves power

bool gottime = false;
RTC_DATA_ATTR uint8_t rtcday;
RTC_DATA_ATTR uint8_t rtcstunde;
RTC_DATA_ATTR uint8_t rtcminute;

// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  gottime = true;
}

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy 7-Segement Clock + Sleep ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();

  // 
  if ((sleepinbetween) && (print_wakeup_reason() == ESP_SLEEP_WAKEUP_TIMER)) {
    wakeuptimerhndlr();
  };

  WiFi.enableIPv6(true);
  // https://www.youtube.com/watch?v=zX9_FyuotR8, timezone lookup: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.nist.gov");
  sntp_set_time_sync_notification_cb(timeavailable);

  setupBoard();
  setupWiFi();
}

//-----------------------------------------------------------------------------------------------
void loop() {
  loopWiFi();
  loopLukeHeapMonitor();
  loopDrawTime();
  // poor mans energy saver
  sleep(1);
}

//