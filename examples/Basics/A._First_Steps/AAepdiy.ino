// here we define the colors our display has
#define white 0xFF
#define black 0x00

// I like to put global variables into struct to keep it together
struct epdiydata_t {
  uint16_t screenwidth = 0;
  uint16_t screenheight = 0;

  EpdiyHighlevelState hl;
  uint8_t* epaperFrameBuffer = NULL;
  EpdFontProperties epaperfont_props;
} epdiydata;

//*********************************************************************************************************************
void setupBoard() {
  epd_init(&epd_board_v7,  // board with esp32-s3
           &ED097TC2,      // epaper: ED097TC2
           EPD_LUT_64K);
  epd_set_vcom(1560);

  epdiydata.hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
  //epd_set_rotation(EPD_ROT_LANDSCAPE);

  epdiydata.epaperFrameBuffer = epd_hl_get_framebuffer(&epdiydata.hl);
  epdiydata.epaperfont_props = epd_font_properties_default();

  epdiydata.screenwidth = epd_width();
  epdiydata.screenheight = epd_height();

  // Serial.println("epaper: " + String(epdiydata.screenwidth) + " w x " + String(epdiydata.screenheight) + " h");

  epd_poweron();
  epd_clear();
  int temperature = epd_ambient_temperature();
  epd_poweroff();

  // Serial.println("current temperature: " + String(temperature));
}

//*********************************************************************************************************************
void epaperUpdateDisplay() {
  epd_poweron();
  int temperature = epd_ambient_temperature();
  EpdDrawError err = epd_hl_update_screen(&epdiydata.hl, MODE_GC16, temperature);
  epd_poweroff();
}





//