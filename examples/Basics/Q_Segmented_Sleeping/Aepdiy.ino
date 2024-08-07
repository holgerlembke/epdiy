// here we define the colors our display has. Great names!
#define white 0xFF
#define black 0x00
#define dark 0xAA
#define light 0x55

// I like to put global variables into struct to keep it together
struct epdiydata_t {
  uint16_t screenwidth = 0;
  uint16_t screenheight = 0;

  EpdiyHighlevelState hl;
  uint8_t* epaperFrameBuffer = NULL;
  EpdFontProperties epaperfont_props;
} epdiydata;

//*********************************************************************************************************************
void setupBoard(bool fromwakeup) {
  epd_init(&epd_board_v7,  // board with esp32-s3
           /*
             Here goes the epaper board name as defined in src/displays.c 
             You should find your board in this list: https://github.com/vroland/epdiy?tab=readme-ov-file#displays
           */
           &ED097TC2,  // epaper: ED097TC2
           /*
             Lookup-Table-Type for the Waveform. Tradeoff between memory usage and speed. 
             Best idea: don't touch it. Play with it after every thing else works.
           */
           EPD_LUT_64K);
  /*
    This sets the display common voltage.
    Usually it is a negative voltage somewhere documented. My ED097TC2 has a sticker on the board reading "-1.59"

    The function parameter is given as a positive value in Millivolts.
  */
  epd_set_vcom(1590);

  epdiydata.hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);

  epdiydata.epaperFrameBuffer = epd_hl_get_framebuffer(&epdiydata.hl);
  epdiydata.epaperfont_props = epd_font_properties_default();

  epdiydata.screenwidth = epd_width();
  epdiydata.screenheight = epd_height();

  if (fromwakeup) {
    // restore the last image
    drawtime();
  } else {
    epd_poweron();
    epd_clear();
    epd_poweroff();
  }
}

//*********************************************************************************************************************
void checkError(enum EpdDrawError err) {
  if (err != EPD_DRAW_SUCCESS) {
    Serial.println("Panic: epdiy error " + String(err));
  }
}

//*********************************************************************************************************************
void epaperUpdateDisplay() {
  epd_poweron();
  int temperature = epd_ambient_temperature();

  // MODE_GC16 hmmmm streifen
  // MODE_GL16 besser, aber löchrig
  // MODE_DU monochrom

  /* 
     EpdDrawError err gives an error result. You might want to check it with checkError();
     EPD_DRAW_SUCCESS is good, everything else not so.
  */
  EpdDrawError err = epd_hl_update_screen(&epdiydata.hl, MODE_DU, temperature);

  epd_poweroff();
}

//