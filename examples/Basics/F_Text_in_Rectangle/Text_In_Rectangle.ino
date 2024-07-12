//*********************************************************************************************************************
void Text_In_Rectangle_Hlpr(int x, int y, char* text) {
  epd_draw_line(0, 0, x, y, black, epdiydata.epaperFrameBuffer);

  const EpdFont* font = &FiraSans_20;
  EpdFontProperties font_props = epd_font_properties_default();

  EpdRect r = epd_get_string_rect(font, text, x, y, 0, &font_props);

  r.x = x;
  r.y = y;
  epd_draw_rect(r, 0, epdiydata.epaperFrameBuffer);

  // here it happens
  r.y += font->advance_y + font->descender;

  epd_write_string(font, text, &x, &r.y, epdiydata.epaperFrameBuffer, &font_props);
}

//*********************************************************************************************************************
void Text_In_Rectangle() {
  Text_In_Rectangle_Hlpr(100, 100, "auo");
  Text_In_Rectangle_Hlpr(100, 200, "YaaTT");
  Text_In_Rectangle_Hlpr(100, 300, "TyhoglyT");

  epaperUpdateDisplay();
}

//
