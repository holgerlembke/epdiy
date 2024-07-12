//*********************************************************************************************************************
// we use dynamic class incarnation (aka "new"), so we should monitor our memory status
inline void loopLukeHeapMonitor() {
  // Wifi-Verbindung
  static uint32_t ticker = 0;

  if (millis() - ticker >= 10000) {
    ticker = millis();
    meminfo();
  }
}

//*********************************************************************************************************************
void meminfo(void) {  // esp32
  static uint32_t lastRAMfreeheap = 0;
  static uint32_t lastRAMgetMaxAllocHeap = 0;
  static uint32_t lastPSRAMfreeheap = 0;
  static uint32_t lastPSRAMgetMaxAllocHeap = 0;

  // great var name selection!
  uint32_t t1 = ESP.getFreeHeap();
  uint32_t t2 = ESP.getMaxAllocHeap();
  uint32_t t3 = ESP.getFreePsram();
  uint32_t t4 = ESP.getMaxAllocPsram();

  if ((t1 != lastRAMfreeheap) || (t2 != lastRAMgetMaxAllocHeap) || (t3 != lastPSRAMfreeheap) || (t4 != lastPSRAMgetMaxAllocHeap)) {
    lastRAMfreeheap = t1;
    lastRAMgetMaxAllocHeap = t2;
    lastPSRAMfreeheap = t3;
    lastPSRAMgetMaxAllocHeap = t4;

    Serial.print("RAM free: ");
    Serial.print(lastRAMfreeheap);
    Serial.print(" B max: ");
    Serial.print(lastRAMgetMaxAllocHeap);
    Serial.print(" B PSRAM free: ");
    Serial.print(lastPSRAMfreeheap);
    Serial.print(" B max: ");
    Serial.print(lastPSRAMgetMaxAllocHeap);
    Serial.println(" B");
  }
}
