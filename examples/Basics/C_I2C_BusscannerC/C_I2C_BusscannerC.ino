/*
  Moin.

  This will scan the I2C-Bus the Arduino-Way with use of esp_ic2_suspend()/esp_i2c_resum()

  Notice the difference: this time *with* epdiy running.

  ReadTheDocs: https://epdiy.readthedocs.io/en/latest/api.html#highlevel-api

  I2C device
  0x20  --> PCA9535        port extender, do not mess with P1x
  0x23  --> BH1750
  0x51  --> PCF8563T       rtc
  0x68      TPS651851RSLR  normally off, so does not show in scan
  0x76  --> bme280

*/
#include <Wire.h>

#include <driver/i2c.h>
#include <epdiy.h>

const uint8_t PinSDA = 39;
const uint8_t PinSCL = 40;

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy I2C Busscanner C");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();

  epd_init(&epd_board_v7, &ED097TC2, EPD_LUT_64K);


  // epd_deinit(); !!!!
}

//-----------------------------------------------------------------------------------------------
void loop() {
  static int counter = 0;
  Serial.print("Scanning #");
  Serial.println(counter++);

  epd_i2c_suspend();
  Wire.begin(PinSDA, PinSCL, 400000);

  for (uint8_t address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("  I2C device: 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    } else if (error == 4) {
      Serial.print("  Error at 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  Wire.end();
  epd_i2c_resume();

  delay(5000);
}

//
