/*
  Moin.

  This example will some text fitting into a rectangle.


  https://epdiy.readthedocs.io/en/latest/api.html
*/

#include <epdiy.h>
#include <fonts/firasans_12.h>   // font includes will be found only after the epdiy-include
#include <fonts/firasans_20.h>


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
  Text_In_Rectangle();
}

//-----------------------------------------------------------------------------------------------
void loop() {}

//