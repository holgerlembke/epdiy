/*
  Moin.

  This will scan the I2C-Bus the Arduino Way

  ReadTheDocs: https://epdiy.readthedocs.io/en/latest/api.html#highlevel-api

  Scanning...
  I2C device: 0x20  --> PCA9535        port extender, do not mess with P1x
  I2C device: 0x51  --> PCF8563T       rtc
              ???       TPS651851RSLR  

*/

#include <Wire.h>
#include <epdiy.h>

const uint8_t PinSDA = 39;
const uint8_t PinSCL = 40;

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy I2C Busscanner A");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
  Serial.println();

  epd_init(&epd_board_v7, &ED097TC2, EPD_LUT_64K);
  // This will not work WITHOUT epd_deinit, because otherwise epd will have the i2c bus under its control.
  epd_deinit();

  Wire.begin(PinSDA, PinSCL, 400000);
}

//-----------------------------------------------------------------------------------------------
void loop() {
  Serial.println("Scanning...");

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

  delay(5000);
}

//