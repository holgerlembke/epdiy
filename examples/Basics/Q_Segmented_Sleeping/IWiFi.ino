//*********************************************************************************************************************
WiFiMulti wifiMulti;

//*********************************************************************************************************************
void setupWiFi() {
  WiFi.mode(WIFI_AP);
  wifiMulti.addAP(privatedata_mySSID, privatedata_myWAP2);
}

//*********************************************************************************************************************
inline void loopWiFi() {
  // Wifi-Verbindung
  static uint32_t ticker = 0;
  const uint32_t connectTimeoutMs = 5000;

  //--------------------------
  if (millis() - ticker >= 1000) {
    ticker = millis();
    if (WiFi.status() != WL_CONNECTED) {
      if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) {
        Serial.print("WiFi connected: ");
        Serial.print(WiFi.SSID());
        Serial.print(" ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("WiFi not connected!");
      }
    }
  }
}

//