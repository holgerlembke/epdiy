void test() {

  epd_draw_line(0, 0, epd_width() - 1, epd_height() - 1, black, epdiydata.epaperFrameBuffer);
  epd_draw_line(0, epd_height() - 1, epd_width() - 1, 0, black, epdiydata.epaperFrameBuffer);

  int r = min(epd_height(), epd_width()) / 2;
  epd_draw_circle(epd_width() / 2, epd_height() / 2, r, black, epdiydata.epaperFrameBuffer);

  epaperUpdateDisplay();
}