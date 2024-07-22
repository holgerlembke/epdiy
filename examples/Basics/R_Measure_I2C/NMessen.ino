//*********************************************************************************************************************
// hier nur die abweichenden Adressen
const uint8_t bh1750addr = 0x23;
const uint8_t bmp280addr = 0x76;

//*********************************************************************************************************************
bool HasDeviceAtI2CAddr(uint8_t addr) {
  enableWire();
  Wire.beginTransmission(addr);
  byte error = Wire.endTransmission();
  disableWire();

  return (error == 0);
}

//*********************************************************************************************************************
BH1750FVI* sensorBH1750 = NULL;
//*********************************************************************************************************************
void setupBH1750() {
  enableWire();
  if (HasDeviceAtI2CAddr(bh1750addr)) {
    sensorBH1750 = new BH1750FVI(bh1750addr);
    sensorBH1750->powerOn();
    sensorBH1750->setContHighRes();
  } else {
    delete sensorBH1750;
    sensorBH1750 = NULL;
    Serial.println("Panic: bh1750 not found.");
  }
  disableWire();
}

//*********************************************************************************************************************
void messungBH1750() {
  const uint32_t zyklus = 15l * 1000l;
  static uint32_t ticker = -zyklus;

  if (millis() - ticker >= zyklus) {
    ticker = millis();
    if (sensorBH1750) {
      enableWire();
      messdatencontainer.bh1750lux = sensorBH1750->getLux();
      messdatencontainer.bh1750valid = true;
      /** /
    Serial.print("BH1750 Brightness: ");
    Serial.print(messdatencontainer.bh1750lux);
    Serial.println(" lux");
    /**/
      disableWire();
    }
  }
}

//*********************************************************************************************************************
Adafruit_BMP280* bmp280 = NULL;
//*********************************************************************************************************************
void setupBMP280() {
  enableWire();
  bmp280 = new Adafruit_BMP280();
  bool status = bmp280->begin(bmp280addr);
  if (!status) {
    delete bmp280;
    bmp280 = NULL;
    Serial.println("Panic: bmp280 not found.");
  } else {
    /* Default settings from datasheet. */
    bmp280->setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  }
  disableWire();
}

//*********************************************************************************************************************
void messungBMP280() {
  const uint32_t zyklus = 15l * 1000l;
  static uint32_t ticker = -zyklus;

  if (millis() - ticker >= zyklus) {
    ticker = millis();
    if (bmp280) {
      enableWire();
      messdatencontainer.bmp280temperatur = bmp280->readTemperature();
      messdatencontainer.bmp280pressure = bmp280->readPressure() / 100.0;
      messdatencontainer.bmp280valid = true;
      /** /
      Serial.print("BMP280 Temperatur: ");
      Serial.print(messdatencontainer.bmp280temperatur);
      Serial.print(" °C Pressure: ");
      Serial.print(messdatencontainer.bmp280pressure);
      Serial.println(" hPa");
      /**/
      disableWire();
    }
  }
}

//*********************************************************************************************************************
SensirionI2CScd4x* scd4x = NULL;
//*********************************************************************************************************************
void setupSCD41() {
  enableWire();

  scd4x = new SensirionI2CScd4x();
  scd4x->begin(Wire);

  bool error = scd4x->stopPeriodicMeasurement();
  if (error) {
    delete scd4x;
    scd4x = NULL;
    Serial.printf("Panic: SCD4X.stopPeriodicMeasurement() error %d\n", error);
  } else {

    uint16_t serial0;
    uint16_t serial1;
    uint16_t serial2;
    error = scd4x->getSerialNumber(serial0, serial1, serial2);
    if (error) {
      Serial.printf("Panic: SCD4X error.getSerialNumber() %d\n", error);
    } else {
      Serial.printf("SCD4X-Serialnumber: %04x %04x %04x\n", serial0, serial1, serial2);
    }

    // Start Measurement
    error = scd4x->startPeriodicMeasurement();
    if (error) {
      Serial.printf("Panic: SCD4X error.startPeriodicMeasurement() %d\n", error);
    }
  }

  disableWire();
}

//*********************************************************************************************************************
void messungSCD4x() {
  const uint32_t zyklus = 15l * 1000l;
  static uint32_t ticker = -zyklus;

  if (millis() - ticker >= zyklus) {
    ticker = millis();
    if (scd4x) {
      enableWire();

      bool error;

      // Druck setzen, wenn vorhanden
      if (messdatencontainer.bmp280valid) {
        error = scd4x->setAmbientPressure((uint16_t)(messdatencontainer.bmp280pressure + 0.5));
        if (error) {
          Serial.printf("Panic: SCD4X error.setAmbientPressure() %d\n", error);
        }
      }

      bool isDataReady = false;
      error = scd4x->getDataReadyFlag(isDataReady);
      if (error) {
        Serial.printf("Panic: SCD4X error.getDataReadyFlag() %d\n", error);
      }
      if (isDataReady) {
        uint16_t co2 = 0;
        error = scd4x->readMeasurement(co2, messdatencontainer.scd4xtemperatur, messdatencontainer.scd4xhumidity);
        messdatencontainer.scd4xco2 = co2;
        messdatencontainer.scd4xvalid = true;
        if (error) {
          Serial.printf("Panic: SCD4X error.readMeasurement() %d\n", error);
        } else if (co2 == 0) {
          Serial.println("Good: SCD4x invalid sample detected, skipping.");
        } else {
          /** /
          Serial.print("SCD4x Co2: ");
          Serial.print(messdatencontainer.scd4xco2, 0);
          Serial.print(" ppm Temperature: ");
          Serial.print(messdatencontainer.scd4xtemperatur);
          Serial.print(" °C Humidity: ");
          Serial.print(messdatencontainer.scd4xhumidity);
          Serial.println(" %RH");
          /**/
        }
      }

      disableWire();
    }
  }
}

//*********************************************************************************************************************
Melopero_APDS9960* apds9960 = NULL;
//*********************************************************************************************************************
void setupAPDS9960() {
  enableWire();

  apds9960 = new Melopero_APDS9960();

  int8_t status = apds9960->initI2C(0x39, Wire);  // Initialize the comunication library
  if (status != NO_ERROR) {
    delete apds9960;
    apds9960 = NULL;
    Serial.println("Panic: apds9960 not found.");
  } else {
    status = apds9960->reset();  // Reset all interrupt settings and power off the device
    if (status != NO_ERROR) {
      delete apds9960;
      apds9960 = NULL;
      Serial.println("Panic: apds9960 error during reset.");
    } else {
      apds9960->enableGesturesEngine();
      apds9960->setGestureProxEnterThreshold(25);
      apds9960->setGestureExitThreshold(20);
      apds9960->setGestureExitPersistence(EXIT_AFTER_4_GESTURE_END);
      apds9960->wakeUp();
    }
  }
  disableWire();
}

//*********************************************************************************************************************
void messungAPDS9960() {
  const uint32_t zyklus = 1000l;
  static uint32_t ticker = -zyklus;

  if (millis() - ticker >= zyklus) {
    ticker = millis();

    if (apds9960) {
      enableWire();
      apds9960->updateGestureStatus();
      if (apds9960->gestureFifoHasData) {
        Serial.print("+");
      }
      disableWire();
    }

    /*
    if (apds9960) {
      uint8_t gesture = apds9960->readGesture();
      if (gesture == APDS9960_DOWN) {
        Serial.println("v")
      } else if (gesture == APDS9960_UP) {
        Serial.println("^")
      } else if (gesture == APDS9960_LEFT) {
        Serial.println("<")
      } else if (gesture == APDS9960_RIGHT) {
        Serial.println(">");
      }
    }
*/
  }
}

//*********************************************************************************************************************
//*********************************************************************************************************************
inline void loopMessungen() {
  const uint32_t zyklus = 1000l;  // 1 second
  static uint32_t ticker = -zyklus;
  const uint16_t diagrammzyklus = 5 * 60;  // seconds
  static uint16_t sekunden = 0;

  if (millis() - ticker >= zyklus) {
    ticker = millis();

    messungBH1750();
    messungBMP280();
    messungSCD4x();
    messungAPDS9960();

    if (gottime && messdatencontainer.bh1750valid && messdatencontainer.bmp280valid && messdatencontainer.scd4xvalid) {
      struct tm ti;
      if (!getLocalTime(&ti)) {
        return;
      }

      uint16_t slot = (ti.tm_min + 60 * ti.tm_hour) / 2;
      temp.adddatapoint(messdatencontainer.bmp280temperatur, slot);
      pressure.adddatapoint(messdatencontainer.bmp280pressure, slot);
      humidity.adddatapoint(messdatencontainer.scd4xhumidity, slot);
      co2.adddatapoint(messdatencontainer.scd4xco2, slot);

      if (sekunden == 0) {
        drawInfoArea();
        drawscreen();
        sekunden = diagrammzyklus;
      } else if ((sekunden % 60) == 0) {
        drawInfoArea();
        updateinfoarea();
        sekunden--;
      } else {
        sekunden--;
      }
    }
  }
}

//*********************************************************************************************************************
uint8_t wirestacker = 0;  // about 250 stacks high...
void disableWire() {
  wirestacker--;
  if (wirestacker == 0) {
    Wire.end();
    epd_i2c_resume();
  }
}

//*********************************************************************************************************************
void enableWire() {
  if (wirestacker == 0) {
    epd_i2c_suspend();
    Wire.begin(PinI2CSDA, PinI2CSCL, 400000);
    wirestacker = 1;
  } else {
    wirestacker++;
  }
}

//