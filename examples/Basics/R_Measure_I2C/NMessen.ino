//*********************************************************************************************************************
// https://github.com/UncleRus/esp-idf-lib/blob/master/examples/bh1750/default/main/main.c
i2c_dev_t bh1750dev;
const uint8_t bh1750addr = 0x23;

//*********************************************************************************************************************
void setupBH1750() {
  memset(&bh1750dev, 0, sizeof(bh1750dev));  // Zero descriptor

  if (ESP_OK != bh1750_init_desc(&bh1750dev, bh1750addr, (i2c_port_t)0, PinI2CSDA, PinI2CSCL)) {
    Serial.println("Panik: BH1750 init failed.");
  }
  if (ESP_OK != bh1750_setup(&bh1750dev, BH1750_MODE_CONTINUOUS, BH1750_RES_HIGH)) {
    Serial.println("Panik: BH1750 setup failed.");
  }
}

//*********************************************************************************************************************
void messungBH1750() {
  uint16_t lux;

  if (bh1750_read(&bh1750dev, &lux) == ESP_OK) {
    // JAA
  } else {
    Serial.println("Panik: BH1750 does not respond.");
  }
}

//