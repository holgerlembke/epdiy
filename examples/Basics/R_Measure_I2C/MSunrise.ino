//*********************************************************************************************************************
// https://arduino.stackexchange.com/questions/30348/format-hour-and-minute-integers-to-hhmm-format-char
String hmtoString(uint8_t hour, uint8_t min) {
  char timestr[] = "--:--\0";
  timestr[0] = '0' + hour / 10;
  timestr[1] = '0' + hour % 10;
  timestr[3] = '0' + min / 10;
  timestr[4] = '0' + min % 10;
  return String(timestr);
}

//*********************************************************************************************************************
String hmtoString(double timepnt) {
  int m = timepnt * 60.0 + 0.5;
  int hr = (m / 60) % 24;
  int mn = m % 60;

  /** /
  Serial.println(timepnt);
  Serial.println(m);
  Serial.println(hr);
  Serial.println(mn);
  /**/

  return hmtoString(hr, mn);
}

//*********************************************************************************************************************
String SonnenAufgangUntergang() {

  struct tm localtimeinfoheute;
  if (getLocalTime(&localtimeinfoheute)) {
    // TimeZone rausfinden
    time_t jetzt;
    time(&jetzt);
    // Kopieren, weil... ach je.
    struct tm utctimeinfoheute;
    memcpy(&utctimeinfoheute, gmtime(&jetzt), sizeof utctimeinfoheute);

    time_t morgen = jetzt + 24 * 60 * 60;  // ein Tag weiter
    struct tm *utctimeinfomorgen = gmtime(&morgen);

    // Korrektur für Tageswechsel nötig
    int timezone = (localtimeinfoheute.tm_hour - utctimeinfoheute.tm_hour);
    if (timezone < 0) {
      timezone += 24;
    }

    double transitheute, sunriseheute, sunsetheute, transitmorgen, sunrisemorgen, sunsetmorgen;

    // Calculate the times of sunrise, transit, and sunset, in hours (UTC)
    calcSunriseSunset(utctimeinfoheute.tm_year + 1900, utctimeinfoheute.tm_mon + 1, utctimeinfoheute.tm_mday, latitude, longitude, transitheute, sunriseheute, sunsetheute);
    calcSunriseSunset(utctimeinfomorgen->tm_year + 1900, utctimeinfomorgen->tm_mon + 1, utctimeinfomorgen->tm_mday, latitude, longitude, transitmorgen, sunrisemorgen, sunsetmorgen);

    /** /
    Serial.println(hmtoString(sunrise + timezone));
    Serial.println(hmtoString(transit + timezone));
    Serial.println(hmtoString(sunset + timezone));
    /**/

    static uint16_t w = 0;
    static uint16_t h = 0;
    static int16_t x1 = 0;
    static int16_t y1 = 0;
    const int8_t hofs = 10;


    //    gfx->setFont(&FreeMono18pt7b);
    //    gfx->setTextColor(WHITE);
    if (h == 0) {
      //      gfx->getTextBounds("88:88", 0, 0, &x1, &y1, &w, &h);
      /** /
          Serial.print("x1: ");
          Serial.print(x1);
          Serial.print(" y1: ");
          Serial.print(y1);
          Serial.print(" w: ");
          Serial.print(w);
          Serial.print(" h: ");
          Serial.println(h);
          /**/
      w = w + 2 * x1 + 32;
      h = h + hofs;
    }
    /** /
    Serial.println(jetzt);
    Serial.println(morgen);
    Serial.println(sunriseheute, 7);
    Serial.println(sunsetheute, 7);
    Serial.println(sunrisemorgen, 7);
    Serial.println(sunsetmorgen, 7);
    Serial.println(utctimeinfoheute.tm_mday);
    Serial.println(utctimeinfoheute.tm_hour);
    Serial.println(utctimeinfoheute.tm_min);
    Serial.println(utctimeinfomorgen->tm_mday);
    /**/

    /*
        zeit berechnen:
          bis zum heutigen Sonnnenaufgang 

jetzt: 1691407935 13:32      su
sonnenuntergangheute: 1691434730
sonnenuntergangmorgen: 1691521018
sonnenaufgangheute: 1691380053
sonnenaufgangmorgen: 1691466549
sonnenuntergangheute diff: 26795           >0
sonnenuntergangmorgen diff: 113083
sonnenaufgangheute diff: -27882            <0 
sonnenaufgangmorgen diff: 58614

jetzt: 1691436641 21:30     sa
sonnenuntergangheute: 1691434730
sonnenuntergangmorgen: 1691521018
sonnenaufgangheute: 1691380053
sonnenaufgangmorgen: 1691466549
sonnenuntergangheute diff: -1911            <0         
sonnenuntergangmorgen diff: 84377
sonnenaufgangheute diff: -56588             <0
sonnenaufgangmorgen diff: 29908

jetzt: 1691452901 2:02      sa
sonnenuntergangheute: 1691521018
sonnenuntergangmorgen: 1691607304
sonnenaufgangheute: 1691466549
sonnenaufgangmorgen: 1691553046
sonnenuntergangheute diff: 68117           >0 
sonnenuntergangmorgen diff: 154403
sonnenaufgangheute diff: 13648             >0
sonnenaufgangmorgen diff: 100145
    */

    time_t sonnenuntergangheute = (jetzt / (24 * 60 * 60)) * (24 * 60 * 60) + (sunsetheute * 60.0 * 60.0 + 0.5);
    time_t sonnenuntergangmorgen = (morgen / (24 * 60 * 60)) * (24 * 60 * 60) + (sunsetmorgen * 60.0 * 60.0 + 0.5);
    time_t sonnenaufgangheute = (jetzt / (24 * 60 * 60)) * (24 * 60 * 60) + (sunriseheute * 60.0 * 60.0 + 0.5);
    time_t sonnenaufgangmorgen = (morgen / (24 * 60 * 60)) * (24 * 60 * 60) + (sunrisemorgen * 60.0 * 60.0 + 0.5);

    static String lasttime = "";
    bool s1 = (sonnenuntergangheute - jetzt < 0);
    bool s2 = (sonnenaufgangheute - jetzt < 0);
    bool sonnenuntergangzuerst = (s1 != s2);

    /** /
    Serial.print("jetzt: ");
    Serial.println(jetzt);
    Serial.print("sonnenuntergangheute: ");
    Serial.println(sonnenuntergangheute);
    Serial.print("sonnenuntergangmorgen: ");
    Serial.println(sonnenuntergangmorgen);
    Serial.print("sonnenaufgangheute: ");
    Serial.println(sonnenaufgangheute);
    Serial.print("sonnenaufgangmorgen: ");
    Serial.println(sonnenaufgangmorgen);
    Serial.print("sonnenuntergangheute diff: ");
    Serial.println(sonnenuntergangheute - jetzt);
    Serial.print("sonnenuntergangmorgen diff: ");
    Serial.println(sonnenuntergangmorgen - jetzt);
    Serial.print("sonnenaufgangheute diff: ");
    Serial.println(sonnenaufgangheute - jetzt);
    Serial.print("sonnenaufgangmorgen diff: ");
    Serial.println(sonnenaufgangmorgen - jetzt);
    /**/

    if (sonnenuntergangzuerst) {
      // Sonnenuntergang + Sonnenaufgang
      return "v" + hmtoString(sunsetheute + timezone) + " ^" + hmtoString(sunrisemorgen + timezone);
    } else {
      // Sonnenaufgang + Sonnenuntergang
      return "^" + hmtoString(sunrisemorgen + timezone) + " v" + hmtoString(sunsetmorgen + timezone);
    }
  }
  return "";
}


//