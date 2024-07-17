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
     0x62  --> SCD41          temp+hum+co2
     0x68  --> TPS651851RSLR  normally off, so does not show in scan
     0x76  --> bmp280         temp+press

*/
#include <FS.h>
#include <FFat.h>

#include <Wire.h>

#include <privatedata.h>  // https://github.com/holgerlembke/privatedata

#include <WiFi.h>
#include <WiFiMulti.h>
#include <NetworkClientSecure.h>
#include <HTTPClient.h>
#include <esp_sntp.h>

#include <driver/i2c.h>
#include <epdiy.h>
#include <fonts/firasans_20.h>
#include <fonts/verdana10.h>
#include <fonts/verdana16.h>
#include <fonts/verdana24.h>

#include <BH1750FVI.h>          // https://github.com/RobTillaart/BH1750FVI_RT
#include <Adafruit_BMP280.h>    // https://github.com/adafruit/Adafruit_BMP280_Library
#include <SensirionI2CScd4x.h>  // https://github.com/Sensirion/arduino-i2c-scd4x

#include <SolarCalculator.h>  // https://github.com/jpb10/SolarCalculator
#include <ArduinoJson.h>      // https://github.com/bblanchon/ArduinoJson
//#include <LinkedList.h>       // https://github.com/ivanseidel/LinkedList


//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy environment monitor ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();

  WiFi.enableIPv6(true);

  setupFS();

  setupTime();
  setupWiFi();
  setupBoard();
  setupDiagramm();

  setupBH1750();
  setupSCD41();
  setupBMP280();
}

//-----------------------------------------------------------------------------------------------
void loop() {
  loopWiFi();
  loopMessungen();
  loopLukeHeapMonitor();
  loopNina();
  delay(1);
}

//
