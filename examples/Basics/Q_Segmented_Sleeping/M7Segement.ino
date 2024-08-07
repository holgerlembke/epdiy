//*********************************************************************************************************************
int dxcalc(int pos, int xdelta, int padding, int dpspace) {
  return pos * (xdelta + 2 * padding) + ((pos >= 2) ? dpspace : 0);
}


//*********************************************************************************************************************
void drawSingleSegment(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3,  // f1
                       uint16_t x4, uint16_t y4, uint16_t x5, uint16_t y5,                            // f2
                       uint16_t x6, uint16_t y6, uint16_t x7, uint16_t y7, uint16_t x8, uint16_t y8,  // f3
                       uint16_t color) {                                                              // color
  epd_fill_triangle(x1, y1, x2, y2, x3, y3, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(x4, y4, x5, y5), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(x6, y6, x7, y7, x8, y8, color, epdiydata.epaperFrameBuffer);
}

//*********************************************************************************************************************
void draw7Segement(uint8_t number, int pos) {
  /*
        a
      f   b
        g  
      e   c
        d

a: 71/56 128/0 413/0 469/56 413/114 128/114
b: 483/70 540/127 540/413 483/469 426/413 426/127
c: 483/497 540/553 540/839 483/895 426/839 426/553
d: 71/909 127/852 413/852 469/909 413/966 127/966
e: 57/497 114/554 114/838 57/895 0/838 0/554
f: 57/70 114/128 114/412 57/469 0/412 0/128
g: 71/483 128/426 413/426 469/483 413/540 127/540

a: xpos[0]/ypos[0] xpos[1]/ypos[1] xpos[2]/ypos[1] xpos[3]/ypos[0] xpos[2]/ypos[2] xpos[1]/ypos[2]
b: xpos[4]/ypos[3] xpos[5]/ypos[4] xpos[5]/ypos[5] xpos[4]/ypos[6] xpos[6]/ypos[5] xpos[6]/ypos[4]
c: xpos[4]/ypos[7] xpos[5]/ypos[8] xpos[5]/ypos[9] xpos[4]/ypos[13] xpos[6]/ypos[9] xpos[6]/ypos[8]
d: xpos[0]/ypos[10] xpos[1]/ypos[11] xpos[2]/ypos[11] xpos[3]/ypos[10] xpos[2]/ypos[12] xpos[1]/ypos[12]
e: xpos[7]/ypos[7] xpos[8]/ypos[8] xpos[8]/ypos[9] xpos[7]/ypos[13] xpos[9]/ypos[9] xpos[9]/ypos[8]
f: xpos[7]/ypos[3] xpos[8]/ypos[4] xpos[8]/ypos[5] xpos[7]/ypos[6] xpos[9]/ypos[5] xpos[9]/ypos[4]
g: xpos[0]/ypos[14] xpos[1]/ypos[15] xpos[2]/ypos[15] xpos[3]/ypos[14] xpos[2]/ypos[16] xpos[1]/ypos[16]
   */

  static EXT_RAM_BSS_ATTR int xpos[] = { 71, 128, 413, 469, 483, 540, 426, 57, 114, 0 };
  static int ypos[] = { 56, 0, 114, 70, 127, 413, 469, 497, 553, 839, 909, 852, 966, 895, 483, 426, 540 };
  static bool isint = false;

  const uint8_t padding = 10;
  const uint8_t dpspace = 80;
  static uint8_t xofs = 0;
  static uint8_t yofs = 0;
  static uint16_t xdelta = 20;

  if (!isint) {
    float scalexy = 0.8;  // overall scaling

    isint = true;
    int ymax = 0;
    int xmax = 0;
    // at this moment I want to note that I use top notch current technology
    for (auto y : ypos) {
      if (y > ymax) {
        ymax = y;
      }
    }
    for (auto x : xpos) {
      if (x > xmax) {
        xmax = x;
      }
    }
    xdelta = scalexy * ((epdiydata.screenwidth) / 4) + 0.5;

    float scale = (float)(epdiydata.screenheight * scalexy - 5 * padding - dpspace) / ymax;
    // tnct ends here.

    // calc xmax/ymax again.
    xmax = 0;
    ymax = 0;
    for (int i = 0; i < sizeof(xpos) / sizeof(xpos[0]); i++) {
      xpos[i] = scalexy * scale * xpos[i] + 0.5 + padding;
      if (xpos[i] > xmax) {
        xmax = xpos[i];
      }
    }
    for (int i = 0; i < sizeof(ypos) / sizeof(ypos[0]); i++) {
      ypos[i] = scalexy * scale * ypos[i] + 0.5 + padding;
      if (ypos[i] > ymax) {
        ymax = ypos[i];
      }
    }

    xofs = (epdiydata.screenwidth - xmax - dxcalc(3, xdelta, padding, dpspace)) / 2;
    yofs = (epdiydata.screenheight - ymax) / 2;

    // draw the :
    const uint16_t dpofs = 40;
    const uint16_t dpxofs = 20;
    const uint16_t dpyofs = 20;
    const uint16_t dpwh = 44;
    epd_fill_rect(makeEpdRect(epdiydata.screenwidth / 2 - dpxofs, epdiydata.screenheight / 2 - dpyofs - dpofs, dpwh, dpwh), black, epdiydata.epaperFrameBuffer);
    epd_fill_rect(makeEpdRect(epdiydata.screenwidth / 2 - dpxofs, epdiydata.screenheight / 2 - dpyofs + dpofs, dpwh, dpwh), black, epdiydata.epaperFrameBuffer);
  }

  static const uint8_t segments[7][3][6] =  //
    {
      { // a
        { 0, 0, 1, 1, 1, 2 },
        { 1, 1, 2, 1, 2, 1 },
        { 2, 1, 3, 0, 2, 2 } },
      { // b
        { 4, 3, 5, 4, 6, 4 },
        { 6, 4, 5, 6, 5, 4 },
        { 5, 5, 4, 6, 6, 5 } },
      { // c
        { 4, 7, 5, 8, 6, 8 },
        { 6, 8, 5, 6, 9, 8 },
        { 5, 9, 4, 13, 6, 9 } },
      { // d
        { 0, 10, 1, 11, 1, 12 },
        { 1, 11, 2, 1, 12, 11 },
        { 2, 11, 3, 10, 2, 12 } },
      { // e
        { 7, 7, 8, 8, 9, 8 },
        { 9, 8, 8, 9, 9, 8 },
        { 8, 9, 7, 13, 9, 9 } },
      { // f
        { 7, 3, 8, 4, 9, 4 },
        { 9, 4, 8, 9, 5, 4 },
        { 8, 5, 7, 6, 9, 5 } },
      { // g
        { 0, 14, 1, 15, 1, 16 },
        { 1, 15, 2, 1, 16, 15 },
        { 2, 15, 3, 14, 2, 16 } }
    };

  /*
    Serial.println("xxx");
    // Hmmm. the for-loop never returns....

  for (uint8_t i = 0; i < 7; i++) {
    epd_fill_triangle(
      xpos[segments[i][0][0]], ypos[segments[i][0][1]],
      xpos[segments[i][0][2]], ypos[segments[i][0][3]],
      xpos[segments[i][0][4]], ypos[segments[i][0][5]], black, epdiydata.epaperFrameBuffer);
    epd_fill_rect(
      makeEpdRect(
        xpos[segments[i][1][0]], ypos[segments[i][1][1]],
        xpos[segments[i][1][2]] - ypos[segments[i][1][3]],
        xpos[segments[i][1][4]] - ypos[segments[i][1][5]]),
      black, epdiydata.epaperFrameBuffer);
    epd_fill_triangle(
      xpos[segments[i][2][0]], ypos[segments[i][2][1]],
      xpos[segments[i][2][2]], ypos[segments[i][2][3]],
      xpos[segments[i][2][4]], ypos[segments[i][2][5]], black, epdiydata.epaperFrameBuffer);
  }

  for (uint8_t i = 0; i < 7; i++) {
    Serial.print(xpos[segments[i][0][0]]);
    Serial.print(" ");
    Serial.print(ypos[segments[i][0][1]]);
    Serial.print(" ");
    Serial.print(xpos[segments[i][0][2]]);
    Serial.print(" ");
    Serial.print(ypos[segments[i][0][3]]);
    Serial.print(" ");
    Serial.print(xpos[segments[i][0][4]]);
    Serial.print(" ");
    Serial.print(ypos[segments[i][0][5]]);
    Serial.println();
  }
*/
  uint16_t dx = xofs;
  uint16_t dy = yofs;

  // shift for digits 0..3
  dx = dx + dxcalc(pos, xdelta, padding, dpspace);

  static const uint8_t digit[11] = {
    //xxBFAEDCGP
    { 0b11111100 },
    { 0b10000100 },
    { 0b10111010 },
    { 0b10101110 },
    { 0b11000110 },
    { 0b01101110 },
    { 0b01111110 },
    { 0b10100100 },
    { 0b11111110 },
    { 0b11101110 },
    { 0b00000000 }
  };

  // a
  uint8_t color;
  color = ((digit[number] & 0b00100000) == 0b00100000) ? black : white;
  drawSingleSegment(xpos[0] + dx, ypos[0] + dy, xpos[1] + dx, ypos[1] + dy, xpos[1] + dx, ypos[2] + dy,
                    xpos[1] + dx, ypos[1] + dy, xpos[2] - xpos[1], ypos[2] - ypos[1],
                    xpos[2] + dx, ypos[1] + dy, xpos[3] + dx, ypos[0] + dy, xpos[2] + dx, ypos[2] + dy, color);
  /*
  epd_fill_triangle(xpos[0] + dx, ypos[0] + dy, xpos[1] + dx, ypos[1] + dy, xpos[1] + dx, ypos[2] + dy, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(xpos[1] + dx, ypos[1] + dy, xpos[2] - xpos[1], ypos[2] - ypos[1]), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(xpos[2] + dx, ypos[1] + dy, xpos[3] + dx, ypos[0] + dy, xpos[2] + dx, ypos[2] + dy, color, epdiydata.epaperFrameBuffer);
*/
  // b
  color = ((digit[number] & 0b10000000) == 0b10000000) ? black : white;
  drawSingleSegment(xpos[4] + dx, ypos[3] + dy, xpos[5] + dx, ypos[4] + dy, xpos[6] + dx, ypos[4] + dy,
                    xpos[6] + dx, ypos[4] + dy, xpos[5] - xpos[6], ypos[5] - ypos[4],
                    xpos[5] + dx, ypos[5] + dy, xpos[4] + dx, ypos[6] + dy, xpos[6] + dx, ypos[5] + dy, color);
  /*
  epd_fill_triangle(xpos[4] + dx, ypos[3] + dy, xpos[5] + dx, ypos[4] + dy, xpos[6] + dx, ypos[4] + dy, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(xpos[6] + dx, ypos[4] + dy, xpos[5] - xpos[6], ypos[5] - ypos[4]), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(xpos[5] + dx, ypos[5] + dy, xpos[4] + dx, ypos[6] + dy, xpos[6] + dx, ypos[5] + dy, color, epdiydata.epaperFrameBuffer);
*/
  // c
  color = ((digit[number] & 0b00000100) == 0b00000100) ? black : white;
  drawSingleSegment(xpos[4] + dx, ypos[7] + dy, xpos[5] + dx, ypos[8] + dy, xpos[6] + dx, ypos[8] + dy,
                    xpos[6] + dx, ypos[8] + dy, xpos[5] - xpos[6], ypos[9] - ypos[8],
                    xpos[5] + dx, ypos[9] + dy, xpos[4] + dx, ypos[13] + dy, xpos[6] + dx, ypos[9] + dy, color);
  /*
  epd_fill_triangle(xpos[4] + dx, ypos[7] + dy, xpos[5] + dx, ypos[8] + dy, xpos[6] + dx, ypos[8] + dy, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(xpos[6] + dx, ypos[8] + dy, xpos[5] - xpos[6], ypos[9] - ypos[8]), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(xpos[5] + dx, ypos[9] + dy, xpos[4] + dx, ypos[13] + dy, xpos[6] + dx, ypos[9] + dy, color, epdiydata.epaperFrameBuffer);
*/
  // d
  color = ((digit[number] & 0b00001000) == 0b00001000) ? black : white;
  drawSingleSegment(xpos[0] + dx, ypos[10] + dy, xpos[1] + dx, ypos[11] + dy, xpos[1] + dx, ypos[12] + dy,
                    xpos[1] + dx, ypos[11] + dy, xpos[2] - xpos[1], ypos[12] - ypos[11],
                    xpos[2] + dx, ypos[11] + dy, xpos[3] + dx, ypos[10] + dy, xpos[2] + dx, ypos[12] + dy, color);
  /*
  epd_fill_triangle(xpos[0] + dx, ypos[10] + dy, xpos[1] + dx, ypos[11] + dy, xpos[1] + dx, ypos[12] + dy, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(xpos[1] + dx, ypos[11] + dy, xpos[2] - xpos[1], ypos[12] - ypos[11]), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(xpos[2] + dx, ypos[11] + dy, xpos[3] + dx, ypos[10] + dy, xpos[2] + dx, ypos[12] + dy, color, epdiydata.epaperFrameBuffer);
*/

  // e
  color = ((digit[number] & 0b00010000) == 0b00010000) ? black : white;
  drawSingleSegment(xpos[7] + dx, ypos[7] + dy, xpos[8] + dx, ypos[8] + dy, xpos[9] + dx, ypos[8] + dy,
                    xpos[9] + dx, ypos[8] + dy, xpos[8] - xpos[9], ypos[9] - ypos[8],
                    xpos[8] + dx, ypos[9] + dy, xpos[7] + dx, ypos[13] + dy, xpos[9] + dx, ypos[9] + dy, color);
  /*
  epd_fill_triangle(xpos[7] + dx, ypos[7] + dy, xpos[8] + dx, ypos[8] + dy, xpos[9] + dx, ypos[8] + dy, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(xpos[9] + dx, ypos[8] + dy, xpos[8] - xpos[9], ypos[9] - ypos[8]), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(xpos[8] + dx, ypos[9] + dy, xpos[7] + dx, ypos[13] + dy, xpos[9] + dx, ypos[9] + dy, color, epdiydata.epaperFrameBuffer);
*/

  // f
  color = ((digit[number] & 0b01000000) == 0b01000000) ? black : white;
  drawSingleSegment(xpos[7] + dx, ypos[3] + dy, xpos[8] + dx, ypos[4] + dy, xpos[9] + dx, ypos[4] + dy,
                    xpos[9] + dx, ypos[4] + dy, xpos[8] - xpos[9], ypos[5] - ypos[4],
                    xpos[8] + dx, ypos[5] + dy, xpos[7] + dx, ypos[6] + dy, xpos[9] + dx, ypos[5] + dy, color);
  /*
  epd_fill_triangle(xpos[7] + dx, ypos[3] + dy, xpos[8] + dx, ypos[4] + dy, xpos[9] + dx, ypos[4] + dy, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(xpos[9] + dx, ypos[4] + dy, xpos[8] - xpos[9], ypos[5] - ypos[4]), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(xpos[8] + dx, ypos[5] + dy, xpos[7] + dx, ypos[6] + dy, xpos[9] + dx, ypos[5] + dy, color, epdiydata.epaperFrameBuffer);
*/

  // g
  color = ((digit[number] & 0b00000010) == 0b00000010) ? black : white;
  drawSingleSegment(xpos[0] + dx, ypos[14] + dy, xpos[1] + dx, ypos[15] + dy, xpos[1] + dx, ypos[16] + dy,
                    xpos[1] + dx, ypos[15] + dy, xpos[2] - xpos[1], ypos[16] - ypos[15],
                    xpos[2] + dx, ypos[15] + dy, xpos[3] + dx, ypos[14] + dy, xpos[2] + dx, ypos[16] + dy, color);
  /*
  epd_fill_triangle(xpos[0] + dx, ypos[14] + dy, xpos[1] + dx, ypos[15] + dy, xpos[1] + dx, ypos[16] + dy, color, epdiydata.epaperFrameBuffer);
  epd_fill_rect(makeEpdRect(xpos[1] + dx, ypos[15] + dy, xpos[2] - xpos[1], ypos[16] - ypos[15]), color, epdiydata.epaperFrameBuffer);
  epd_fill_triangle(xpos[2] + dx, ypos[15] + dy, xpos[3] + dx, ypos[14] + dy, xpos[2] + dx, ypos[16] + dy, color, epdiydata.epaperFrameBuffer);
*/
}

//*********************************************************************************************************************
void drawtime() {
  draw7Segement(rtcstunde / 10, 0);
  draw7Segement(rtcstunde % 10, 1);
  draw7Segement(rtcminute / 10, 2);
  draw7Segement(rtcminute % 10, 3);
  epaperUpdateDisplay();
}

//*********************************************************************************************************************
void drawtimeemptydisplay(int v) {
  draw7Segement(v, 0);
  draw7Segement(v, 1);
  draw7Segement(v, 2);
  draw7Segement(v, 3);
}

//*********************************************************************************************************************
void sleepUntilNextMinuteChange() {
  struct tm ti;
  if (getLocalTime(&ti)) {
    ItsBedTime(60 - ti.tm_sec - 1);
  }
}

//*********************************************************************************************************************
inline void loopDrawTime() {
  // Wifi-Verbindung
  static uint32_t ticker = 0;

  if (millis() - ticker >= 1000) {
    ticker = millis();

    struct tm ti;
    if (!getLocalTime(&ti)) {
      Serial.println("Time: no time available");
      return;
    }

    if ((rtcstunde != ti.tm_hour) || (rtcminute != ti.tm_min)) {
      rtctag = ti.tm_mday;
      rtcminute = ti.tm_min;
      rtcstunde = ti.tm_hour;
      drawtime();

      // go to sleep?
      if ((sleepinbetween) && (gottime)) {
        sleepUntilNextMinuteChange();
      }
    }
  }
}

//*********************************************************************************************************************
void wakeuptimerhndlr() {
  struct tm ti;
  if (!getLocalTime(&ti)) {
    return;
  }

  // Tageswechsel. Wifi erzwingen
  if (rtctag != ti.tm_mday) {
    rtctag = ti.tm_mday;
    return;
  }

  // wait until next minute
  /** /
  Serial.print("rtcmemory: ");
  Serial.print(rtcstunde);
  Serial.print(" ");
  Serial.print(rtcminute);
  Serial.print(" ti: ");
  Serial.print(ti.tm_hour);
  Serial.print(" ");
  Serial.println(ti.tm_min);
  /**/
  while ((rtcminute == ti.tm_min) && (rtcstunde == ti.tm_hour)) {
    delay(100);
    getLocalTime(&ti);
  }
  rtcminute = ti.tm_min;
  rtcstunde = ti.tm_hour;
  drawtime();
  sleepUntilNextMinuteChange();
}

//