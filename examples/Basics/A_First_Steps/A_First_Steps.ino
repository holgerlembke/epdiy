/*
  Moin.

  This is the first step in using epdiy (https://github.com/vroland/epdiy) within the Arduino IDE.
  It is my idea how Arduino programs should by structured. You might have a different idea.

  Goal:
    - get the hardware configured
    - include epdiy
    - have a tab with all the stuff to get the display running (easy copy-paste-solution)
    - paint some lines and a circle 

  Hardware:
    This guide assumes the use of the "Epdiy V7" board with a ESP32-S3-Wroom-N16R8 
    see https://vroland.github.io/epdiy-hardware/

  Arduino-Board-Settings:
     Board: ESP32S3 Dev Modul
     Flash Size: 16 MB 
     Part Scheme: 16 MB Flash (2 MB App, 12,5 MB FFATFS) 
               or 16 MB Flash (3 MB App, 9.6 MB FFATFS) 
     PSRAM: OPI PSRAM

     (everything else is default for this modul)


  Compile order for tabs:
     The Arduino IDE compiles the sketches/tab like this:
       - all tab content is added together
         -- first the main tab (always on the left side.)
         -- the other tabs in alphabetically ASCII order
            (so ABC...abc!)
       - while doing that, all function calls are collected and inserted 
         after the last #include in the main tab
     This results in a .cpp file that is passed to the c++ compiler.      

*/

#include <epdiy.h>

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(100); }
  Serial.print(F("\n\epdiy First"));
  Serial.print(F(__DATE__));
  Serial.print(" ");
  Serial.print(F(__TIME__));
  Serial.println();

  setupBoard();
  test();
}

//-----------------------------------------------------------------------------------------------
void loop() {}

//