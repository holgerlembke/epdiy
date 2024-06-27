/*
  Moin.

  If you made changes within the first examples "Aepdiy.ino" please copy the changes to this example.

  This example will draw black/white 3x3 pattern and let the centre rectangle blink.
*/

#include <epdiy.h>

//-----------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  do { delay(100); } while (!Serial);
  Serial.print("\nepdiy Second ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.print(__TIME__);
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