//*********************************************************************************************************************
void test1() {

  int xofs = epd_width() / 3;
  int yofs = epd_height() / 3;

  EpdRect rect = {
    .x = 0,
    .y = 0,
    .width = xofs,
    .height = yofs
  };

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      rect.x = i * xofs;
      rect.y = j * yofs;
      epd_fill_rect(rect, ((i * 3 + j) % 2 == 0) ? black : white, epdiydata.epaperFrameBuffer);
    }
  }

  epaperUpdateDisplay();
}

//*********************************************************************************************************************
void test2() {
  static bool schwippschwapp = false;  // this is a valid German word

  int xofs = epd_width() / 3;
  int yofs = epd_height() / 3;

  EpdRect rect = {
    .x = xofs,
    .y = yofs,
    .width = xofs,
    .height = yofs
  };

  epd_fill_rect(rect, schwippschwapp ? black : white, epdiydata.epaperFrameBuffer);
  schwippschwapp = !schwippschwapp;

  epaperUpdateDisplay();
}

//