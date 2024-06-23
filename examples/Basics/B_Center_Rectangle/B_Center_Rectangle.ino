/*
  Moin.

  If you made changes within the first examples "Aepdiy.ino" please copy the changes to this example.

  This example will draw black/white 3x3 pattern and let the centre rectangle blink.
*/

#include <epdiy.h>

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(100); }
  Serial.print(F("\n\epdiy Second"));
  Serial.print(F(__DATE__));
  Serial.print(" ");
  Serial.print(F(__TIME__));
  Serial.println();

  setupBoard();
  test1();
}

//-----------------------------------------------------------------------------------------------
void loop() {
  delay(1000);
  test2();
}

//