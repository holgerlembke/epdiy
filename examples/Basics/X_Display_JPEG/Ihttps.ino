//*********************************************************************************************************************
void loopURLGetter() {
  // Wifi-Verbindung
  static uint32_t ticker = 0;
  static uint16_t seconds = nextimage + 1;
  static uint16_t errordelay = 10;
  static uint16_t errorinc = 100;  // 10

  //--------------------------
  if (millis() - ticker >= 1000) {
    ticker = millis();
    seconds++;
    if ((seconds >= nextimage) && (WiFi.status() == WL_CONNECTED)) {
      if (GetURLAndDisplay(pictureurl)) {
        seconds = 0;
        errordelay = 10;
      } else {
        // in 10 Sekunden noch mal probieren
        seconds = nextimage - errordelay;
        // langsam noch langsamer werden
        if (errordelay < nextimage) {
          errordelay += errorinc;
        }
      }
    }
  }
}

//*********************************************************************************************************************
struct jpegglobal_t {
  int deltax;
  int deltay;
  uint8_t gamme_curve[256];
};

jpegglobal_t *jpegglobal = NULL;

//*********************************************************************************************************************
// Affects the gamma to calculate gray (lower is darker/higher contrast)
// Nice test values: 0.9 1.2 1.4 higher and is too bright
double gamma_value = 0.5;

//*********************************************************************************************************************
void makegammacurve() {
  double gammaCorrection = 1.0 / gamma_value;
  for (int gray_value = 0; gray_value < 256; gray_value++)
    jpegglobal->gamme_curve[gray_value] = round(255 * pow(gray_value / 255.0, gammaCorrection));
}

//*********************************************************************************************************************
/*inline?*/ void checkandpaint(int16_t x, int16_t y, uint8_t col) {
  x += jpegglobal->deltax;
  y += jpegglobal->deltay;
  if ((x >= 0) && (y >= 0) && (x < epdiydata.screenwidth) && (y < epdiydata.screenheight)) {
    epd_draw_pixel(x, y, col, epdiydata.epaperFrameBuffer);
  }
}

//*********************************************************************************************************************
int drawstyle1(JPEGDRAW *pDraw) {
  int iCount;
  iCount = pDraw->iWidth * pDraw->iHeight;  // number of pixels to draw in this call

  for (int16_t xx = 0; xx < pDraw->iWidth; xx += 4) {
    for (int16_t yy = 0; yy < pDraw->iHeight; yy++) {
      uint16_t col = pDraw->pPixels[(xx + (yy * pDraw->iWidth)) >> 2];

      uint8_t col1 = col & 0xf;
      uint8_t col2 = (col >> 4) & 0xf;
      uint8_t col3 = (col >> 8) & 0xf;
      uint8_t col4 = (col >> 12) & 0xf;
      checkandpaint(pDraw->x + xx, pDraw->y + yy, jpegglobal->gamme_curve[col1 * 16]);
      checkandpaint(pDraw->x + xx + 1, pDraw->y + yy, jpegglobal->gamme_curve[col2 * 16]);
      checkandpaint(pDraw->x + xx + 2, pDraw->y + yy, jpegglobal->gamme_curve[col3 * 16]);
      checkandpaint(pDraw->x + xx + 3, pDraw->y + yy, jpegglobal->gamme_curve[col4 * 16]);
    }
  }
  return 1;  // 1->continue
}

//*********************************************************************************************************************
// from JPEGDEC\examples\epd_demo\epd_demo.ino
int drawstyle2(JPEGDRAW *pDraw) {
  int x = pDraw->x;
  int y = pDraw->y;
  int w = pDraw->iWidth;
  int h = pDraw->iHeight;

  for (int i = 0; i < w * h; i++) {
    pDraw->pPixels[i] = (pDraw->pPixels[i] & 0x7e0) >> 5;  // extract just the six green channel bits.
  }

  for (int16_t j = 0; j < h; j++) {
    for (int16_t i = 0; i < w; i++) {
      int8_t oldPixel = constrain(pDraw->pPixels[i + j * w], 0, 0x3F);
      int8_t newPixel = oldPixel & 0x38;  // or 0x30 to dither to 2-bit directly. much improved tonal range, but more horizontal banding between blocks.
      pDraw->pPixels[i + j * w] = newPixel;
      int quantError = oldPixel - newPixel;
      if (i + 1 < w) pDraw->pPixels[i + 1 + j * w] += quantError * 7 / 16;
      if ((i - 1 >= 0) && (j + 1 < h)) pDraw->pPixels[i - 1 + (j + 1) * w] += quantError * 3 / 16;
      if (j + 1 < h) pDraw->pPixels[i + (j + 1) * w] += quantError * 5 / 16;
      if ((i + 1 < w) && (j + 1 < h)) pDraw->pPixels[i + 1 + (j + 1) * w] += quantError * 1 / 16;
    }  // for i
  }    // for j

  for (int16_t i = 0; i < w; i++) {
    for (int16_t j = 0; j < h; j++) {
      switch (constrain(pDraw->pPixels[i + j * w] >> 4, 0, 3)) {
        case 0:
          checkandpaint(x + i, y + j, black);
          break;
        case 1:
          checkandpaint(x + i, y + j, dark);
          break;
        case 2:
          checkandpaint(x + i, y + j, light);
          break;
        case 3:
          checkandpaint(x + i, y + j, white);
          break;
      }  // switch
    }    // for j
  }      // for i
  return 1;
} /* JPEGDraw() */


//*********************************************************************************************************************
void printLasterrorString(int error) {
  Serial.print(error);
  Serial.print("=");
  switch (error) {
    case JPEG_SUCCESS:
      {
        Serial.print("JPEG_SUCCESS");
        break;
      }
    case JPEG_INVALID_PARAMETER:
      {
        Serial.print("JPEG_INVALID_PARAMETER");
        break;
      }
    case JPEG_DECODE_ERROR:
      {
        Serial.print("JPEG_DECODE_ERROR");
        break;
      }
    case JPEG_UNSUPPORTED_FEATURE:
      {
        Serial.print("JPEG_UNSUPPORTED_FEATURE");
        break;
      }
    case JPEG_INVALID_FILE:
      {
        Serial.print("JPEG_INVALID_FILE");
        break;
      }
    default:
      {
        Serial.print("unknown");
        break;
      }
  }
}

//*********************************************************************************************************************
// this is a modified version of BasicHttpsClient from ESP32 https://github.com/espressif/arduino-esp32
// current problem: https://github.com/espressif/arduino-esp32/issues/9924#issuecomment-2185642278
// benötigt WiFi.enableIPv6(true);
bool GetURLAndDisplay(String URL) {
  bool ok = false;
  NetworkClientSecure *client = new NetworkClientSecure;
  if (client) {
    client->setInsecure();

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before NetworkClientSecure *client is
      HTTPClient *https = new HTTPClient;
      //https->setUserAgent("User-Agent Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:127.0) Gecko/20100101 Firefox/127.0");
      //https->setReuse(false);
      https->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

      if (https->begin(*client, URL)) {  // this is a define...
        Serial.print("[HTTPS] GET: ");
        Serial.print(URL);
        // start connection and send HTTP header
        int httpCode = https->GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.print(" httpCode: ");
          Serial.print(httpCode);

          if (httpCode == HTTP_CODE_OK) {  // 200
            // now we have the image
            int size = https->getSize();
            Serial.print(" Size: ");
            Serial.print(size);

            if (size > 0) {
              uint8_t *buffer = (uint8_t *)ps_malloc(size);
              if (buffer) {
                NetworkClient *stream = https->getStreamPtr();
                stream->readBytes(buffer, size);

                uint8_t *dither_space = (uint8_t *)ps_malloc(epd_width() * 16);
                if (dither_space) {
                  JPEGDEC *jpeg = new JPEGDEC;
                  if (jpeg->openRAM(buffer, size, drawstyle2)) {
                    Serial.print(" JPEG-Decode: openRAM ok, JPEG: ");
                    Serial.print(jpeg->getWidth());
                    Serial.print("x");
                    Serial.print(jpeg->getHeight());
                    Serial.print(" Delta: ");

                    jpegglobal = (jpegglobal_t *)malloc(sizeof(jpegglobal_t));

                    // this will center the image on the display
                    jpegglobal->deltax = (epdiydata.screenwidth - jpeg->getWidth()) / 2;
                    jpegglobal->deltay = (epdiydata.screenheight - jpeg->getHeight()) / 2;
                    Serial.print(jpegglobal->deltax);
                    Serial.print("x");
                    Serial.print(jpegglobal->deltay);
                    Serial.print(" ");

                    makegammacurve();

                    epd_fill_rect(makeEpdRect(0, 0, epdiydata.screenwidth, epdiydata.screenheight), white, epdiydata.epaperFrameBuffer);
                    //jpeg->setPixelType(FOUR_BIT_DITHERED);
                    jpeg->decodeDither(dither_space, 0);

                    // yeah!
                    epaperUpdateDisplay();

                    free(jpegglobal);
                    jpegglobal = NULL;

                    // yeah!!
                    ok = true;

                    if (sleepinbetween) {
                      //!!!!!!!!!!!!!!!!!!!!!!!
                      ItsBedTime(nextimage);
                    }
                  } else {
                    Serial.print(" JPEG-Decode: openRAM failed. Last error: ");
                    printLasterrorString(jpeg->getLastError());
                    DumpJPEGBuffer(buffer, min(size, 1000));
                  }

                  delete jpeg;
                  free(buffer);
                } else {
                  Serial.print(" JPEG-Decode: out of PSRAM for JPEG buffer");
                }
              } else {
                Serial.print(" JPEG-Decode: out of dither RAM");
              }
            } else {
              Serial.print(". Ignoring, only size>0 can be handled.");
            }
          }
          Serial.println();
        } else {
          Serial.print(" failed with httpCode ");
          Serial.print(httpCode);
          Serial.print(" = ");
          Serial.println(https->errorToString(httpCode));
        }

        https->end();
      } else {
        Serial.println("[HTTPS] Unable to connect");
      }

      delete https;
      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  return ok;
}

//*********************************************************************************************************************
// fetches a webpage. more or less the same as before...
String GetHTMLFromURL(String URL) {
  String res = "";

  NetworkClientSecure *client = new NetworkClientSecure;
  if (client) {
    client->setInsecure();
    {
      HTTPClient *https = new HTTPClient;
      https->setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      if (https->begin(*client, URL)) {  // HTTPS
        int httpCode = https->GET();
        if (httpCode == 200) {
          res = https->getString();
        } else {
          Serial.print("  failed  (2) with httpCode ");
          Serial.print(httpCode);
          Serial.print(" = ");
          Serial.println(https->errorToString(httpCode));
        }
        https->end();
      } else {
        Serial.println("[HTTPS] Unable to connect (2)");
      }
      delete https;
    }
    delete client;
  } else {
    Serial.println("Unable to create client (2)");
  }
  return res;
}

// Helper: dumps the receiving memory buffer
void DumpJPEGBuffer(uint8_t *buffer, uint32_t len) {
  uint32_t c = 0;
  const uint8_t w = 32;
  while (len > 0) {
    if (c % w == 0) {
      Serial.printf("\n%04x ", c);
    }
    Serial.printf("%02x ", *buffer);
    buffer++;
    len--;
    c++;
  }
  Serial.println();
}

// Helper: List the JPEG-Markers, does not work...
// https://en.wikipedia.org/wiki/JPEG_File_Interchange_Format#File_format_structure
// https://cyber.meme.tips/jpdump/#
void DumpJPEGMarkers(uint8_t *buffer, uint32_t len) {
  if (*buffer == 0xff) {
    buffer++;
    if (*buffer == 0xd8) {
      len -= 2;
      Serial.println("FF D8 Startmarker");
      buffer++;

      do {
        if (*buffer == 0xff) {
          buffer++;
          len--;
          uint8_t marker = *buffer;
          buffer++;
          len--;
          // restart-marker
          if ((marker >= 0xd0) && (marker <= 0xd7)) {
            Serial.printf("ff %02x Restart-Marker, Len = 0\n", marker);
          } else
            // end-marker
            if (marker == 0xd9) {
              Serial.printf("ff %02x End-Marker, Len = 0\n", marker);
            } else
              // define restart-marker
              if (marker == 0xdd) {
                Serial.printf("ff %02x Define-Restart Marker, Len = 4\n", marker);
                buffer += 4;
                len -= 4;
              } else {
                // variable size marker
                uint32_t s1 = *buffer;
                buffer++;
                len--;
                uint32_t s2 = *buffer;
                buffer++;
                len--;
                uint32_t databytes = 256 * s1 + s2 - 2;
                buffer += databytes;
                len -= databytes;
                Serial.printf("ff %02x Marker, Len = %d\n", marker, databytes);
              }
        } else {
          Serial.print("End of Markers, len left: ");
          Serial.println(len);
          return;
        }
      } while (len > 0);
    }
  }
}

/*
0000 ff d8 ff e1 00 de 45 78 69 66 00 00 49 49 2a 00 08 00 00 00 06 00 12 01 03 00 01 00 00 00 01 00 
0020 00 00 1a 01 05 00 01 00 00 00 56 00 00 00 1b 01 05 00 01 00 00 00 5e 00 00 00 28 01 03 00 01 00 
0040 00 00 02 00 00 00 13 02 03 00 01 00 00 00 01 00 00 00 69 87 04 00 01 00 00 00 66 00 00 00 00 00 
0060 00 00 48 00 00 00 01 00 00 00 48 00 00 00 01 00 00 00 07 00 00 90 07 00 04 00 00 00 30 32 31 30 
0080 01 91 07 00 04 00 00 00 01 02 03 00 86 92 07 00 16 00 00 00 c0 00 00 00 00 a0 07 00 04 00 00 00 
00a0 30 31 30 30 01 a0 03 00 01 00 00 00 ff ff 00 00 02 a0 04 00 01 00 00 00 b0 04 00 00 03 a0 04 00 
00c0 01 00 00 00 39 03 00 00 00 00 00 00 41 53 43 49 49 00 00 00 50 69 63 73 75 6d 20 49 44 3a 20 31 
00e0 38 36 ff db 00 43 00 08 06 06 07 06 05 08 07 07 07 09 09 08 0a 0c 14 0d 0c 0b 0b 0c 19 12 13 0f 
0100 14 1d 1a 1f 1e 1d 1a 1c 1c 20 24 2e 27 20 22 2c 23 1c 1c 28 37 29 2c 30 31 34 34 34 1f 27 39 3d 
*/

//
