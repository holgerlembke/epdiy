/*
  Moin.

  If you made changes within the first examples "Aepdiy.ino" please copy the changes to this example.

  do {
    In this example we will display a simple 7-segement clock and do sleep.
    Additional, we only log into WiFi once a day to same energy.
  } while (1);  
*/

// the wifi connection
#include <WiFi.h>
#include <WiFiMulti.h>
#include <sntp.h>             // nonos-sdk

// we want to do https:
#include <NetworkClientSecure.h>
#include <HTTPClient.h>

// a concept to keep your credentials out of your sketches
// you can remove this and add your ssid/passwort into IWiFi.ino
#include <privatedata.h>  // https://github.com/holgerlembke/privatedata

#include <epdiy.h>

// <<<Configuration>>>
const uint16_t nextimage = 60 * 10;  // in Seconds. This (10 min) is too fast for a picture frame thingy.
const bool sleepinbetween = false;   // set to sleep, saves power

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy 7-Segement Clock + Sleep ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();

  print_wakeup_reason();

  WiFi.enableIPv6(true);
  // Keine Zeitzone nötig
  configTime(0, 0, "pool.ntp.org");

  setupBoard();
  setupWiFi();

  drawtime();
}

void loop() {
  sleep(1);
}

//-----------------------------------------------------------------------------------------------
void loop2() {
  loopWiFi();
  loopLukeHeapMonitor();

  // poor mans energy saver
  sleep(1);
}

//