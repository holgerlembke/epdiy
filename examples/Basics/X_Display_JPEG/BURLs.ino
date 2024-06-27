// *******************
// *******************
// *******************
// Edit here to use one of the Picture Services below
#define pictureurl NasaPictureOfTheDay()
// *******************
// *******************
// *******************

/*
     Important notice:
         Image decoding only works for non-progressive encoded jpegs.

         Some services use progressive encoded jpegs (this does not work!), 
         progressive encoded images will fail during decode. Got it?

         Usually you will see a "Last error: 3=JPEG_UNSUPPORTED_FEATURE" and a dump of some of the jpeg.

         The display process will simply retry with the next image. If that fails it will wait
         a little bit longer and try aqain the next image. Rinse, repeat.
*/

// Cats As A Service
String CatsAsService() {
  return "https://cataas.com/cat";
}

// Random Nasa picture of the day
// sometimes we get .gif, skip that
String NasaPictureOfTheDay() {
  int limit = 10;
  do {
    String t = NasaPictureOfTheDaySub();
    String tl = t;
    tl.toLowerCase();
    if (tl.endsWith(".jpg")) {
      return t;
    }
    limit--;
  } while (limit > 0);

  return "";
}

/*
URL: https://apod.nasa.gov/apod/ap050130.html
[HTTPS] GET: https://apod.nasa.gov/image/0204/teapotsirds_winfree.gif httpCode: 200 Size: 242015 JPEG-Decode: openRAM failed. Last error: 4=JPEG_INVALID_FILE
0000 47 49 46 38 39 61 71 02 a6 01 f7 ff 00 4e f3 1d a5 9e ca 9a 24 8b df f0 48 27 d5 6f d3 d2 ae 58 
*/
inline String NasaPictureOfTheDaySub() {
  // Read the source of https://apod.nasa.gov/apod/fap/random_apod.html and you will understand...
  String base = "https://apod.nasa.gov/apod/apYYMMDD.html";

  time_t start = 800928000l;  // 1995-05-20 00:00:00
  time_t now;
  time(&now);

  time_t rtime = random(now - start) + start;
  struct tm* ts = gmtime(&rtime);

  /** /
  Serial.print((ts->tm_year + 1900) % 100);
  Serial.print(" ");
  Serial.print(ts->tm_mon);
  Serial.print(" ");
  Serial.println(ts->tm_mday);
  /**/

  byte y = (ts->tm_year + 1900) % 100;
  base.replace("YY", (y < 10 ? "0" : "") + String(y));
  base.replace("MM", (ts->tm_mon < 9 ? "0" : "") + String(ts->tm_mon + 1));
  base.replace("DD", (ts->tm_mday < 10 ? "0" : "") + String(ts->tm_mday));

  /**/
  Serial.print("URL: ");
  Serial.println(base);
  /**/

  String wpage = GetHTMLFromURL(base);
  // Serial.println(wpage);

  // <IMG SRC = "image/1402/m44franke900.jpg"
  int i = wpage.indexOf("<IMG SRC=");
  if (i > 0) {
    wpage.remove(0, i + 1 + 9);
    i = wpage.indexOf("\"");
    if (i > 0) {
      wpage.remove(i);
      // Serial.println(wpage);
      return "https://apod.nasa.gov/" + wpage;
    }
  }

  return "";  //
}



// Lorem Picsum, Lorem Ipsum for Pictures. Does not work, progressive encoded!
String LoremPicsum() {
  String base = "https://picsum.photos/W/H";

  base.replace("/W", "/" + String(epdiydata.screenwidth));
  base.replace("/H", "/" + String(epdiydata.screenheight));

  //String wpage = GetHTMLFromURL(base);
  //Serial.println(wpage);

  return base;
}
