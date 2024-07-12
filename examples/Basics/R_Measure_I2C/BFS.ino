
//*********************************************************************************************************************
void setupFS() {
  if (!FFat.begin()) {
    FFat.format();
    Serial.println("FFat formatted.");
    if (!FFat.begin()) {
      Serial.println("FFat failed.");
    }
  }
  Serial.printf("FFat Total: %10u Bytes, Free: %10u Bytes\n", FFat.totalBytes(), FFat.freeBytes());
  listDir(FFat, "/", 42);
}

//*********************************************************************************************************************
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

////