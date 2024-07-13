// this is defined in epdiy\src\board\epd_board_v7.c
const uint8_t PinI2CSCL = 40;
const uint8_t PinI2CSDA = 39;


const String City = "Braunschweig";
const String Country = "DE";
// Location
double latitude = -1000.0;   // 52.2689;
double longitude = -1000.0;  // 10.5268;

const String regionalschuessel = "031010000000";                                       //"091620000000"; // "031010000000";
const String ninaapiURL = "https://nina.api.proxy.bund.dev/api31/dashboard/%1%.json";  // %1%: Gemeindeschlüssel


// %1% --> apikey
// #define privatedata_openweatherapikey = "ffff"
const String OpenweathermapURI = "http://api.openweathermap.org/data/2.5/forecast?lat=%2%&lon=%3%&appid=%1%&lang=de&units=metric";
const String OpenweatherMapLocationURI = "http://api.openweathermap.org/geo/1.0/direct?q=%2%&limit=5&appid=%1%";
const String OpenweathermapIconPathURI = "http://openweathermap.org/img/wn/%1%.png";


// Container für die Messdaten
struct messdatencontainer_t {
  bool bh1750valid = false;
  float bh1750lux;

  bool bmp280valid = false;
  float bmp280temperatur;
  float bmp280pressure;

  bool scd4xvalid = false;
  float scd4xtemperatur;
  float scd4xhumidity;
  float scd4xco2;

} messdatencontainer;



//