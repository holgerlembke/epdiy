//*********************************************************************************************************************
/* 

  https://nina.api.bund.dev/

  https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json           ???

  https://nina.api.proxy.bund.dev/api31/warnings/mow.DE-NI-BS-W091-20240715-000.json

  https://www.xrepository.de/api/xrepository/urn:de:bund:destatis:bevoelkerungsstatistik:schluessel:rs_2024-07-31/download/Regionalschl_ssel_2024-07-31.json



https://nina.api.proxy.bund.dev/api31/dashboard/031010000000.json
https://warnung.bund.de/bbk.mowas/gefahrendurchsagen.json
https://www.bbk.bund.de/DE/Infothek/Fachinformationsstelle/fachinformationsstelle_node.html
https://github.com/joergschultzelutter/mowas-pwb/blob/master/src/mowas.py
https://nina.api.bund.dev/
https://www.xrepository.de/api/xrepository/urn:de:bund:destatis:bevoelkerungsstatistik:schluessel:rs_2024-07-31/download/Regionalschl_ssel_2024-07-31.json
https://www.destatis.de/DE/Themen/Laender-Regionen/Regionales/Gemeindeverzeichnis/_inhalt.html
https://www.statistikportal.de/de/gemeindeverzeichnis
https://www.google.com/search?client=firefox-b-d&sca_esv=b52209c30933552b&sca_upv=1&sxsrf=ADLYWILfY10sAaCa011_rGSgTWrRREzG3g:1721136251456&q=Gemeindeschl%C3%BCssel+abfragen+%22json%22&sa=X&ved=2ahUKEwiD5YKX1KuHAxV-S_EDHbw2AmAQ5t4CegQIJxAB&biw=1313&bih=960&dpr=1.71
https://github.com/bundesAPI/nina-api/issues/24
https://arduinojson.org/v6/assistant/#/step4

*/

//*********************************************************************************************************************
String RegionalSchluessel2GemeindeSchluessel(String rs) {
  return rs.substring(0, 5) + "0000000";  // 5+7=12
  /*
  const string regionalschuessel = "031010000000";
                      // auf Kreisebene: 0000000
                      //            012345678901
*/
}

//*********************************************************************************************************************
String RequestWarnmeldungen(String gs) {
  String res = ninaapiURL;
  res.replace("%1%", gs);
  return res;
}

//*********************************************************************************************************************
int NinaListLength = -1;
String* NinaList = NULL;        // new String[n];
const uint8_t NinaListIPI = 4;  // ListItemsPerIncident
/* 
    0 : betreff 
    1 : beginn 
    2 : ende 
    3 : beschreibung(detail)
*/

//*********************************************************************************************************************
void Ninatesterdata() {
  NinaListLength = 5;
  NinaList = new String[NinaListLength * NinaListIPI];

  for (int i = 0; i < NinaListLength; i++) {
    NinaList[i * NinaListIPI + 0] = "DWD: TEST " + String(i) + " Amtliche WARNUNG vor STURMBOEEN";
    NinaList[i * NinaListIPI + 1] = "Beginn: 2023-08-07 10:00:00";
    NinaList[i * NinaListIPI + 2] = "Ende: 2023-08-07 21:00:00";
    NinaList[i * NinaListIPI + 3] = "Beschreibung zu dem Event. Beschreibung zu dem Event. Beschreibung zu dem Event. Beschreibung zu dem Event.";
  }
}

/*
Nina.size(): 1
NinaDetail: https://nina.api.proxy.bund.dev/api31/warnings/dwd.2.49.0.0.276.0.DWD.PVW.1721573340000.8f86d3c2-72fe-4d15-a5ae-985bf9f5a1d3.MUL.json
Description: Von Südwesten ziehen Gewitter auf. Dabei gibt es heftigen Starkregen mit Niederschlagsmengen um 40 l/m² pro Stunde sowie Sturmböen mit Geschwindigkeiten um 75 km/h (21 m/s, 41 kn, Bft 9) und Hagel mit Korngrößen um 2 cm.

*/

//*********************************************************************************************************************
void DeAmtliche() {
  for (int i = 0; i < NinaListLength; i++) {
    String s = NinaList[i * NinaListIPI + 0];
    int idx = s.indexOf("Amtliche ");
    if (idx>-1) {
      s.remove(idx,9);
      NinaList[i * NinaListIPI + 0]=s;
    }
  }
}
 
//*********************************************************************************************************************
String HandleBeginnEnde(String s) {
  if (s != "null") {
    s = s.substring(0, 19);
    s.replace("T", " ");
    return s;
  }

  return "";
}

//*********************************************************************************************************************
void NinaWarnmeldungsHandler() {

  String URL = RequestWarnmeldungen(RegionalSchluessel2GemeindeSchluessel(regionalschuessel));
  // https://nina.api.proxy.bund.dev/api31/dashboard/031010000000.json

  Serial.print("Nina: ");
  Serial.println(URL);

  String warnmeldung = GetHTMLFromURL(URL);

  // überprüfen! https://arduinojson.org/v6/assistant/
  StaticJsonDocument<2 * 2048> doc;

  DeserializationError error = deserializeJson(doc, warnmeldung);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  Serial.print("Nina.size(): ");
  Serial.println(doc.size());

  // alte Liste weg und Liste erzeugen
  if (NinaList) {
    delete[] NinaList;
  }
  if (doc.size() == 0) {
    NinaList = NULL;
    NinaListLength = -1;

    //Ninatesterdata();
  } else {
    NinaListLength = doc.size();
    NinaList = new String[NinaListLength * NinaListIPI];

    for (int i = 0; i < doc.size(); i++) {
      String id = doc[i]["id"];
      String msg = doc[i]["i18nTitle"]["de"];
      String provider = doc[i]["payload"]["data"]["provider"];

      // begin kann null sein, z.b. bei aufhebung einer warnung
      String Beginn = doc[i]["onset"];
      String Ende = doc[i]["expires"];

      Beginn = HandleBeginnEnde(Beginn);
      Ende = HandleBeginnEnde(Ende);

      NinaList[i * NinaListIPI + 0] = provider + ": " + msg;
      NinaList[i * NinaListIPI + 1] = (Beginn == "") ? "" : "Beginn: " + Beginn;
      NinaList[i * NinaListIPI + 2] = (Ende == "") ? "" : "Ende: " + Ende;
      NinaList[i * NinaListIPI + 3] = "";

      /** /
      Serial.print("id: ");
      Serial.println(id);
      Serial.print("Msg: ");
      Serial.println(msg);
      Serial.print("Provider: ");
      Serial.println(provider);
      Serial.print("Beginn: ");
      Serial.println(Beginn);
      Serial.print("Ende: ");
      Serial.println(Ende);
      /**/

      NinaWarnmeldungsHandlerDetails(id, i * 3);
    }
    DeAmtliche();
  }
}

//*********************************************************************************************************************
void NinaWarnmeldungsHandlerDetails(String id, int NinaListIdx) {
  String URL = ninaapiDetailURL;
  URL.replace("%1", id);

  Serial.print("NinaDetail: ");
  Serial.println(URL);

  String warnmeldung = GetHTMLFromURL(URL);

  StaticJsonDocument<2 * 3072> doc;

  DeserializationError error = deserializeJson(doc, warnmeldung);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject info = doc["info"][0];
  NinaList[NinaListIdx + 3] = (String)info["description"];
  /**/
  Serial.print("Description: ");
  Serial.println(NinaList[NinaListIdx + 3]);
  /**/
}

//*********************************************************************************************************************
void loopNina() {
  const uint32_t zyklus = 5 * 60l * 1000l;  // all 5 minutes
  static uint32_t ticker = -zyklus;

  if (millis() - ticker >= zyklus) {
    ticker = millis();
    if (WiFi.status() == WL_CONNECTED) {
      NinaWarnmeldungsHandler();
    }
  }
}

//
