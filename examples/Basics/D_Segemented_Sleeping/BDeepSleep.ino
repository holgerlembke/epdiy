//*********************************************************************************************************************
void ItsBedTime(uint32_t sleeptime) {         // in Seconds, https://www.youtube.com/watch?v=yzjv-WdDaMo
  const uint64_t uS_TO_S_FACTOR = 1000000ULL; /* Conversion factor for micro seconds to seconds */
  /*
     let us sleep one day.
        24 * 60 * 60 * 1000000 --> 3.17+4.09+4.09+13.8 = 25.2/0.7 = 36.3 ~ 37 bit, all good.
  */

  esp_sleep_enable_timer_wakeup((uint64_t)sleeptime * uS_TO_S_FACTOR);
  Serial.flush();
  esp_deep_sleep_start();
  // we are gone.
}

//*********************************************************************************************************************
// from Timerwakeup example
esp_sleep_wakeup_cause_t print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: wakeup_reason = ESP_SLEEP_WAKEUP_UNDEFINED; break;
  }
  return wakeup_reason;
}

//