bool gottime = false;

//*********************************************************************************************************************
// Callback function (gets called when time adjusts via NTP)
void timeavailable(struct timeval *t) {
  gottime = true;
}

//*********************************************************************************************************************
void setupTime() {
  // https://www.youtube.com/watch?v=zX9_FyuotR8, timezone lookup: https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org", "time.nist.gov");
  sntp_set_time_sync_notification_cb(timeavailable);
}

//*********************************************************************************************************************
inline void loopMessung() {
  const uint32_t timeframe = 2l * 60l * 1000l;
  static uint32_t ticker = -timeframe;

  if (millis() - ticker >= timeframe) {
    ticker = millis();
    drawscreen();
  }
}

//*********************************************************************************************************************
inline void loopFakeMessung() {
  const uint32_t timeframe = 1000l;
  static uint32_t ticker = -timeframe;

  if (millis() - ticker >= timeframe) {
    ticker = millis();

    temp.adddatapoint(20.0+random(0,20)/10.0);
    pressure.adddatapoint(1000.0+random(0,40));
    humidity.adddatapoint(50.0+random(0,10));
    co2.adddatapoint(400.0+random(0,1220));
    brightness = 1000 + random(1, 100);
  }
}
//