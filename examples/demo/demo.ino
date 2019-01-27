#include <TM1629Ext.h>
#include "Wire.h"

// I/O pins on the Arduino connected to strobe, clock, data
// (power should go to 3.3v and GND)
Tm1629Ext tm(8, 9, 10);

void setup() {
  Serial.begin(115200);

  tm.begin();

  //for(int i=0;i<16;i++){
  //
  //  tm.displayHex(i,15-i);
  //  }
  Serial.println(Wire.getClock(), DEC);
}

void loop() {

  tm.displayText("Hello World");
  //Serial.println(tm.readButtons(),BIN);
  static uint32_t oldBtns = tm.readButtons();
  uint32_t Btns = tm.readButtons();
  uint32_t changes = (Btns ^ oldBtns);
  if (changes) {
    for (uint8_t i = 0; i < 32; i++) {
      uint32_t mask = (0x00000001 << i);
      if (mask & changes) {
        if (Btns & mask) {
          Serial.print("Key Pressed:");
          Serial.println(i + 1);
        }

        else {
          Serial.print("Key Released:");
          Serial.println(i + 1);
        }


      }

    }
    oldBtns = Btns;

  }

}

