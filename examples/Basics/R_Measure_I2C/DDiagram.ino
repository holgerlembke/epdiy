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
  void adddatapoint(float datum) {  // first datum is the baseline
    if (!valid) {
      for (int i = 0; i < valuepoints; i++) {
        value[i] = datum;
      }
      valid = true;
    } else {
      value[idx++] = datum;
      if (idx >= valuepoints) {
        idx = 0;
      }
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

    int y = diagramheight - 1 - (data.value[i] - data.min) / (data.max - data.min) * (diagramheight - 2);
    epd_draw_line(i + 1, yofs + diagramheight - 1, i + 1, y + yofs, black, epdiydata.epaperFrameBuffer);
  }

  EpdFontProperties font_props = epd_font_properties_default();
  {
    const EpdFont* font = &OpenSans24;
    // Diagram title
    EpdRect r = epd_get_string_rect(font, data.title.c_str(), 0, 0, 0, &font_props);
    r.x = valuepoints - 1 - r.width;
    r.y = yofs + 1;
    // clear background
    epd_fill_rect(r, white, epdiydata.epaperFrameBuffer);
    r.y += font->advance_y + font->descender;
    epd_write_string(font, data.title.c_str(), &r.x, &r.y, epdiydata.epaperFrameBuffer, &font_props);
  }

  {
    const EpdFont* font = &OpenSans16;
    // upper/lower limits
    String limit = String(data.max, (unsigned int)data.digits);
    EpdRect r = epd_get_string_rect(font, limit.c_str(), 0, 0, 0, &font_props);
    r.x = valuepoints + 2;
    r.y = yofs;
    epd_fill_rect(r, white, epdiydata.epaperFrameBuffer);
    r.y += font->advance_y + font->descender;
    epd_write_string(font, limit.c_str(), &r.x, &r.y, epdiydata.epaperFrameBuffer, &font_props);

    limit = String(data.min, (unsigned int)data.digits);
    r = epd_get_string_rect(font, limit.c_str(), 0, 0, 0, &font_props);
    r.x = valuepoints + 2;
    r.y = yofs + diagramheight - 1 - font->advance_y - font->descender;
    epd_fill_rect(r, white, epdiydata.epaperFrameBuffer);
    r.y += font->advance_y + font->descender;
    epd_write_string(font, limit.c_str(), &r.x, &r.y, epdiydata.epaperFrameBuffer, &font_props);
  }
}

//*********************************************************************************************************************
void drawscreen() {
  drawDiagram(0 * diagramheight, temp, 2);
  drawDiagram(1 * diagramheight, pressure, 10);
  drawDiagram(2 * diagramheight, humidity, 5);
  drawDiagram(3 * diagramheight, co2, 100);

  epaperUpdateDisplay();
}

//*********************************************************************************************************************
void setupDiagramm() {
  temp.title = "° C";
  pressure.title = "hPa";
  humidity.title = "%";
  co2.title = "ppm";

  //??
}



//