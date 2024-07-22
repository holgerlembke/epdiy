//*********************************************************************************************************************
void Text_In_Rectangle_Hlpr(int x, int y, char* text) {
  epd_draw_line(0, 0, x, y, black, epdiydata.epaperFrameBuffer);

  const EpdFont* font = &FiraSans_20;
  EpdFontProperties font_props = epd_font_properties_default();

  EpdRect r = epd_get_string_rect(font, text, x, y, 0, &font_props);
  Serial.print("x: ");
  Serial.print(r.x);
  Serial.print(" y: ");
  Serial.print(r.y);
  Serial.print(" w: ");
  Serial.print(r.width);
  Serial.print(" h: ");
  Serial.print(r.height);
  Serial.println();

  r.x = x;
  r.y = y;
  epd_draw_rect(r, 0, epdiydata.epaperFrameBuffer);

  // here it happens
  r.y += font->advance_y + font->descender;

  epd_write_string(font, text, &x, &r.y, epdiydata.epaperFrameBuffer, &font_props);
}

//*********************************************************************************************************************
void TextWrapHlpr(int x, int y, int w, char* text) {
  epd_draw_line(0, 0, x, y, black, epdiydata.epaperFrameBuffer);

  const EpdFont* font = &FiraSans_20;
  EpdFontProperties font_props = epd_font_properties_default();

  EpdRect r = epd_TextWrap(font, text, x, y, w, epdiydata.epaperFrameBuffer, &font_props);
  epd_draw_rect(r, 0, epdiydata.epaperFrameBuffer);
}

//*********************************************************************************************************************
void Text_In_Rectangle() {
  Text_In_Rectangle_Hlpr(100, 100, "auo");
  Text_In_Rectangle_Hlpr(100, 200, "YaaTT");
  Text_In_Rectangle_Hlpr(100, 300, "TyhoglyT");

  Text_In_Rectangle_Hlpr(1000, 400, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
  TextWrapHlpr(500, 500, epdiydata.screenwidth - 900, "abc def ghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");

  epaperUpdateDisplay();
}

//
