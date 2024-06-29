//*********************************************************************************************************************
void Text_In_Rectangle_Hlpr(int x, int y, String text) {
  const EpdFont* font = &FiraSans_20;

  const int margin = 2;

  EpdRect r = epd_get_string_rect(font, text.c_str(), 0, 0, margin, &epdiydata.epaperfont_props);

  // let us move it to 100/100 and paint the rectangle
  r.x = x;
  r.y = y;
  epd_draw_rect(r, black, epdiydata.epaperFrameBuffer);

  // and now move the margin down and draw the text into the box
  r.y += r.height;
  epd_write_string(font, text.c_str(), &r.x, &r.y, epdiydata.epaperFrameBuffer, &epdiydata.epaperfont_props);
}


//*********************************************************************************************************************
void Text_In_Rectangle() {
  Text_In_Rectangle_Hlpr(100, 100, "hallo");
  Text_In_Rectangle_Hlpr(100, 200, "YygTT");

  epaperUpdateDisplay();
}

//