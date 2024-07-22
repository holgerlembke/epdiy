/*
 we are 1200 x 825

 a measurement every minute gives 1440 data points, 720 every 2 minutes.

 480 remain.

 data values
   temp             
   pressure
   humidity
   co2
   brightness   kein diagram
*/

const int valuepoints = 720;
const int diagramheight = 820 / 4 + 1;
const float minvalue = -1000.0;

const EpdFont* fontl = &Verdana24;
const EpdFont* fonts = &Verdana16;
const EpdFont* fontNl = &Verdana16;
const EpdFont* fontNm = &Verdana12;
const EpdFont* fontNs = &Verdana10;

//*********************************************************************************************************************
class datacontainer {
public:
  float value[valuepoints];
  float min, max;
  int idx;
  String title;
  bool valid;
  uint8_t digits;  // overloading

  //--------------------------------------------
  datacontainer() {
    idx = 0;
    for (int i = 0; i < valuepoints; i++) {
      value[i] = minvalue - 1;
    }
  }

  //--------------------------------------------
  void prepdraw(int normalize) {
    min = 10000.0;  // include <float.h> and use flt_max?
    max = -10000.0;
    for (float datum : value) {
      if (datum > minvalue) {
        if (datum < min) {
          min = datum;
        }
        if (datum > max) {
          max = datum;
        }
      }
    }

    min = (int)(min / normalize) * normalize;
    max = (int)(max / normalize + 1) * normalize;
    /** /
    Serial.print(title);
    Serial.print(" Min: ");
    Serial.print(min);
    Serial.print(" Max: ");
    Serial.println(max);
    /**/
  }

  //--------------------------------------------
  void adddatapoint(float datum, uint16_t dptime) {  // first datum is the baseline
    if (!valid) {
      for (int i = 0; i < valuepoints; i++) {
        value[i] = datum;
      }
      valid = true;
    } else {
      value[dptime] = datum;
    }
  }
};

datacontainer temp;
datacontainer pressure;
datacontainer humidity;
datacontainer co2;
float brightness;

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
void drawDiagram(int yofs, datacontainer& data, int normalize /**/) {  // do not generate forward, IDE
  data.prepdraw(normalize);

  if (!data.valid) {
    return;
  }

  EpdRect rect = makeEpdRect(0, yofs, valuepoints + 2, diagramheight);
  epd_draw_rect(rect, black, epdiydata.epaperFrameBuffer);

  // draw hour lines, 3/6/9/etc is longer
  int hourd = 4;
  for (int i = 0; i < 24; i++) {
    epd_draw_line(i * 30, yofs + 1, i * 30, yofs + 15 + (i % 3 == 0 ? 10 : 0), black, epdiydata.epaperFrameBuffer);
  }

  for (int i = 0; i < valuepoints; i++) {
    // white line to make space
    epd_draw_line(i + 1, yofs + 1, i + 1, yofs + diagramheight - 2, white, epdiydata.epaperFrameBuffer);

    const uint8_t griddy = 3;

    int y = diagramheight - 1 - (data.value[i] - data.min) / (data.max - data.min) * (diagramheight - 2);
    epd_draw_line(i + 1, y + yofs + 2, i + 1, y + yofs, black, epdiydata.epaperFrameBuffer);
    for (uint16_t ys = y + yofs + 2 + (i % griddy); ys < yofs + diagramheight; ys += griddy) {
      epd_draw_pixel(i + 1, ys, black, epdiydata.epaperFrameBuffer);
    }
  }

  EpdFontProperties font_props = epd_font_properties_default();
  {
    // Diagram title
    EpdRect r = epd_get_string_rect(fontl, data.title.c_str(), 0, 0, 0, &font_props);
    r.x = valuepoints - 1 - r.width;
    r.y = yofs + 1;
    // clear background
    epd_fill_rect(r, white, epdiydata.epaperFrameBuffer);
    r.y += fontl->advance_y + fontl->descender;
    epd_write_string(fontl, data.title.c_str(), &r.x, &r.y, epdiydata.epaperFrameBuffer, &font_props);
  }

  {
    // upper/lower limits
    String limit = String(data.max, (unsigned int)data.digits);
    EpdRect r = epd_get_string_rect(fonts, limit.c_str(), 0, 0, 0, &font_props);
    r.x = valuepoints + 2;
    r.y = yofs;
    epd_fill_rect(r, white, epdiydata.epaperFrameBuffer);
    r.y += fonts->advance_y + fonts->descender;
    epd_write_string(fonts, limit.c_str(), &r.x, &r.y, epdiydata.epaperFrameBuffer, &font_props);

    limit = String(data.min, (unsigned int)data.digits);
    r = epd_get_string_rect(fonts, limit.c_str(), 0, 0, 0, &font_props);
    r.x = valuepoints + 2;
    r.y = yofs + diagramheight - 1 - fonts->advance_y - fonts->descender;
    epd_fill_rect(r, white, epdiydata.epaperFrameBuffer);
    r.y += fonts->advance_y + fonts->descender;
    epd_write_string(fonts, limit.c_str(), &r.x, &r.y, epdiydata.epaperFrameBuffer, &font_props);
  }
}

//*********************************************************************************************************************
void drawscreen() {
  // Langzeit-Diagramm
  drawDiagram(0 * diagramheight, temp, 2);
  drawDiagram(1 * diagramheight, pressure, 10);
  drawDiagram(2 * diagramheight, humidity, 5);
  drawDiagram(3 * diagramheight, co2, 100);

  // Kurzzeit-Diagramm?

  epaperUpdateDisplay();

  Serial.println("Full update done.");
}

//*********************************************************************************************************************
EpdRect InfoAreaRect() {
  int xofs = valuepoints + 100;
  return makeEpdRect(xofs, 0, epdiydata.screenwidth - xofs, epdiydata.screenheight);
}

//*********************************************************************************************************************
void drawPart(String s1, String s2, uint16_t xpos, uint16_t ypos) {
  EpdFontProperties font_props = epd_font_properties_default();
  EpdRect rt = epd_get_string_rect(fontl, s1.c_str(), 0, 0, 0, &font_props);
  rt.x = xpos;
  rt.y = ypos;
  epd_write_string(fontl, s1.c_str(), &rt.x, &rt.y, epdiydata.epaperFrameBuffer, &font_props);
  rt.x = xpos + rt.width + 10;
  rt.y = ypos;
  epd_write_string(fonts, s2.c_str(), &rt.x, &rt.y, epdiydata.epaperFrameBuffer, &font_props);
}

//*********************************************************************************************************************
void drawInfoAreaMessdaten(uint16_t xofs, uint16_t yofs) {
  EpdFontProperties font_props = epd_font_properties_default();

  // fetch height
  EpdRect baser = epd_get_string_rect(fontl, "xyT", 0, 0, 0, &font_props);

  int xpos = xofs;
  int ypos = baser.height + 2 + yofs;

  const uint8_t dd = 12;
  drawPart(String(messdatencontainer.bmp280temperatur, 1), temp.title, xpos, ypos);
  ypos += baser.height - dd;
  drawPart(String(messdatencontainer.bmp280pressure, 0), pressure.title, xpos, ypos);
  ypos += baser.height - dd;
  drawPart(String(messdatencontainer.scd4xhumidity, 0), humidity.title, xpos, ypos);
  ypos += baser.height - dd;
  drawPart(String(messdatencontainer.scd4xco2, 0), co2.title, xpos, ypos);
  ypos += baser.height - dd;
  drawPart(String(messdatencontainer.bh1750lux, 2), "lux", xpos, ypos);

  Serial.println("Infoarea Messdaten update done.");
}

//*********************************************************************************************************************
void drawInfoAreaTime(uint16_t xofs, uint16_t yofs) {
  const String days[] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa" };
  struct tm ti;
  if (!getLocalTime(&ti)) {
    return;
  }

  EpdFontProperties font_props = epd_font_properties_default();
  // fetch height
  EpdRect baser = epd_get_string_rect(fontl, "xyT", 0, 0, 0, &font_props);
  int xpos = xofs;
  int ypos = baser.height + 2 + yofs;

  String s = hmtoString(ti.tm_hour, ti.tm_min);
  epd_write_string(fontl, s.c_str(), &xpos, &ypos, epdiydata.epaperFrameBuffer, &font_props);

  baser = epd_get_string_rect(fonts, "xyT", 0, 0, 0, &font_props);
  xpos = xofs;
  ypos -= baser.height - 10;
  s = days[ti.tm_wday] + " ";
  s += String(ti.tm_mday) + ".";
  s += String(ti.tm_mon + 1) + ".";
  s += String(ti.tm_year + 1900);
  epd_write_string(fonts, s.c_str(), &xpos, &ypos, epdiydata.epaperFrameBuffer, &font_props);

  s = SonnenAufgangUntergang();
  xpos = xofs;
  ypos -= 11;
  epd_write_string(fonts, s.c_str(), &xpos, &ypos, epdiydata.epaperFrameBuffer, &font_props);
}

//*********************************************************************************************************************
void drawNinaNachrichten(uint16_t xofs, uint16_t yofs) {
  static int NinaListScrollIdx = 0;

  Serial.println("start draw nina info.");

  if (NinaList) {
    int idx = NinaListScrollIdx * NinaListIPI;

    EpdFontProperties font_props = epd_font_properties_default();
    // fetch height
    EpdRect baser = epd_get_string_rect(fontNl, "xyT", 0, 0, 0, &font_props);
    int xpos = xofs;
    int ypos = baser.height + 2 + yofs;

    EpdRect r = epd_TextWrap(fontNl, NinaList[idx + 0].c_str(), xpos, ypos, epdiydata.screenwidth - xofs, epdiydata.epaperFrameBuffer, &font_props);
    ypos += r.height;
    if (NinaList[1] != "") {
      r = epd_TextWrap(fontNs, NinaList[idx + 1].c_str(), xpos, ypos, epdiydata.screenwidth - xofs, epdiydata.epaperFrameBuffer, &font_props);
      ypos += r.height;
    }
    if (NinaList[2] != "") {
      r = epd_TextWrap(fontNs, NinaList[idx + 2].c_str(), xpos, ypos, epdiydata.screenwidth - xofs, epdiydata.epaperFrameBuffer, &font_props);
      ypos += r.height;
    }
    r = epd_TextWrap(fontNm, NinaList[idx + 3].c_str(), xpos, ypos, epdiydata.screenwidth - xofs, epdiydata.epaperFrameBuffer, &font_props);

    NinaListScrollIdx++;
    if (NinaListScrollIdx >= NinaListLength) {
      NinaListScrollIdx = 0;
    }
  } else {
    NinaListScrollIdx = 0;
  }

  Serial.println("draw nina info done.");
}

//*********************************************************************************************************************
void drawInfoArea() {
  EpdRect area = InfoAreaRect();
  // Bereich weiß anmalen
  epd_fill_rect(area, white, epdiydata.epaperFrameBuffer);

  drawInfoAreaTime(area.x, 0);
  drawInfoAreaMessdaten(area.x, 130);
  drawNinaNachrichten(area.x, 360);
}

//*********************************************************************************************************************
void updateinfoarea() {
  epd_poweron();
  int temperature = epd_ambient_temperature();
  EpdDrawError err = epd_hl_update_area(&epdiydata.hl, MODE_DU, temperature, InfoAreaRect());
  epd_poweroff();
}

//*********************************************************************************************************************
void setupDiagramm() {
  temp.title = "°C";
  pressure.title = "hPa";
  humidity.title = "%RH";
  co2.title = "ppm";

  //??
}

//*********************************************************************************************************************
// debug.
EpdRect epd_TextWrap2(const EpdFont* font, const char* string, int x, int y, int w, uint8_t* framebuffer, const EpdFontProperties* properties) {
  y += font->advance_y + font->descender;

  EpdRect r;
  r.x = x;
  r.y = y;

  char s[] = " ";  // single char+\0

  const char* t = string;
  int xx = x;
  int yy = y;
  int sw = x + w;
  while (*t) {
    s[0] = *t++;
    EpdRect rr = epd_get_string_rect(font, s, xx, yy, 0, properties);
    if (xx + rr.width > sw) {
      // führende leerschritte nach Zeilenumbruch überspringen. Wenn also ein Leerschritt am Ende umgebrochen werden muss, einfach weitermachen
      if (s[0] == ' ') {
        continue;
      }
      xx = x;
      y += rr.height;
      yy = y;
    }
    epd_write_string(font, s, &xx, &yy, framebuffer, properties);
    yy = y;
  }
  r.height = yy - r.y + font->advance_y;
  r.width = w;
  r.y -= (font->advance_y + font->descender);

  return r;
}

//