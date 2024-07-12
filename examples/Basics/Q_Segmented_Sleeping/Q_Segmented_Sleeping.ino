/*
  Moin.

  If you made changes within the first examples "Aepdiy.ino" please copy the changes to this example.

  do {
    In this example we will display a simple 7-segement clock and do sleep.
    Additional, we only log into WiFi once a day to save energy.
  } while (1);  

  ReadTheDocs: https://epdiy.readthedocs.io/en/latest/api.html#highlevel-api

  More about the rtc:
     https://deepbluembedded.com/esp32-change-cpu-speed-clock-frequency/
     https://github.com/espressif/esp-idf/issues/6687    autocalc.zip
     https://github.com/atomic14/diy-esp32-epub-reader

*/

// the wifi connection
#include <WiFi.h>
#include <WiFiMulti.h>
#include <esp_sntp.h>

// a concept to keep your credentials out of your sketches
// you can remove this and add your ssid/passwort into IWiFi.ino
#include <privatedata.h>  // https://github.com/holgerlembke/privatedata

#include <epdiy.h>

// <<<Configuration>>>
const bool sleepinbetween = true;  // set to sleep, saves power

bool gottime = false;
RTC_DATA_ATTR uint8_t rtctag;
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

  // immer.
  setupBoard(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER);

  // https://www.youtube.com/watch?v=zX9_FyuotR8, timezone lookup: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.nist.gov");
  sntp_set_time_sync_notification_cb(timeavailable);

  // from Sleep?
  if ((sleepinbetween) && (print_wakeup_reason() == ESP_SLEEP_WAKEUP_TIMER)) {
    wakeuptimerhndlr();
  };

  WiFi.enableIPv6(true);

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