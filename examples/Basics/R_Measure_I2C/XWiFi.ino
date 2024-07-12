//*********************************************************************************************************************
WiFiMulti wifiMulti;

//*********************************************************************************************************************
void setupWiFi() {
  wifiMulti.addAP(privatedata_mySSID, privatedata_myWAP2);
  /*
    wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
    wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
    */
}

//*********************************************************************************************************************
inline void loopWiFi() {
  // Wifi-Verbindung
  const uint32_t tframe = 1000;
  static uint32_t ticker = -tframe;
  const uint32_t connectTimeoutMs = 5000;

  //--------------------------
  if (millis() - ticker >= tframe) {
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

//*********************************************************************************************************************
String GetHTMLFromURL(String URL) {
  String res = "";

  NetworkClientSecure *client = new NetworkClientSecure;
  if (client) {
    client->setInsecure();
    {
      HTTPClient *https = new HTTPClient;
      https->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      if (https->begin(*client, URL)) {  // HTTPS
        int httpCode = https->GET();
        if (httpCode == 200) {
          res = https->getString();
        } else {
          Serial.print("  failed  (2) with httpCode ");
          Serial.print(httpCode);
          Serial.print(" = ");
          Serial.println(https->errorToString(httpCode));
        }
        https->end();
      } else {
        Serial.println("[HTTPS] Unable to connect (2)");
      }
      delete https;
    }
    delete client;
  } else {
    Serial.println("Unable to create client (2)");
  }
  return res;
}

//