/*
  Moin.

  This will scan the I2C-Bus the IDF Way

  Notice the difference: this time *with* epdiy running.

  ReadTheDocs: https://epdiy.readthedocs.io/en/latest/api.html#highlevel-api

  I2C device
  0x20  --> PCA9535        port extender, do not mess with P1x
  0x23  --> BH1750
  0x51  --> PCF8563T       rtc
  0x68      TPS651851RSLR  normally off, so does not show in scan
  0x76  --> bme280

*/
#include <driver/i2c.h>
#include <epdiy.h>

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy I2C Busscanner B");
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

  for (uint8_t address = 1; address < 127; address++) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1) | I2C_MASTER_WRITE, 1);  // /* expect ack
    i2c_master_stop(cmd);
    esp_err_t res = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (res == 0) {
      Serial.print("  I2C device: 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  delay(5000);
}

//
