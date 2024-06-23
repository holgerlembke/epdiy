/*
  Moin.

  This is the first step in using epdiy (https://github.com/vroland/epdiy) within the Arduino IDE.
  It is my idea how Arduino programs should by structured. You might have a different idea.

  Goal:
    - include epdiy
    - have a tab with all the stuff to get the display running (easy copy-paste-solution)
    - paint some lines and a circle (to test)

*/

#include <epdiy.h>

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(100);
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