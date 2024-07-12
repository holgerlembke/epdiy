/*
  Moin.

  This is an environment monitor. With Nina-Services. And Sunup/down-Clock.

  ReadTheDocs: https://epdiy.readthedocs.io/en/latest/api.html#highlevel-api

  For the sensors I took the code from
     https://github.com/UncleRus/esp-idf-lib
  and copied them into a sub-folder sensors. Then I sprinkled a lavish number of "sensors/" into the includes.

  I2C devices
     0x20  --> PCA9535        port extender, do not mess with P1x
     0x23  --> BH1750
     0x51  --> PCF8563T       rtc
     0x68  --> TPS651851RSLR  normally off, so does not show in scan
     0x76  --> bme280

*/
#include <FS.h>
#include <FFat.h>

#include <privatedata.h>                    // https://github.com/holgerlembke/privatedata

#include <WiFi.h>
#include <WiFiMulti.h>
#include <NetworkClientSecure.h>
#include <HTTPClient.h>
#include <esp_sntp.h>

#include <driver/i2c.h>
#include <epdiy.h>
#include <fonts/firasans_20.h>
#include <fonts/opensans16.h>
#include <fonts/opensans24.h>

#include <SolarCalculator.h>                // https://github.com/jpb10/SolarCalculator
#include <ArduinoJson.h>                    // https://github.com/bblanchon/ArduinoJson
#include <LinkedList.h>                     // https://github.com/ivanseidel/LinkedList


//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy environment monitor ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();

  setupFS();

  setupTime();
  setupWiFi();
  setupBoard();
  setupDiagramm();

  setupBH1750();
}

//-----------------------------------------------------------------------------------------------
void loop() {
  loopMessung();
  loopLukeHeapMonitor();
  loopFakeMessung();
  loopWiFi();
  delay(1);
}

//
