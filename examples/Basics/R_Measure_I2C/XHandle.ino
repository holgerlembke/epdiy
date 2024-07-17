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

//