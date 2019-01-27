#include <TM1629Ext.h>
#include "Wire.h"

// I/O pins on the Arduino connected to strobe, clock, data
// (power should go to 3.3v and GND)
TM1629Ext tm(8, 9, 10);

void setup() {
  Serial.begin(115200);

  tm.begin();

  //for(int i=0;i<16;i++){
  //
  //  tm.displayHex(i,15-i);
  //  }
  Serial.println(Wire.getClock(), DEC);

  tm.displayNumber(24000, 350, -1000, 10);
  //Serial.println(tm.readButtons(),BIN);
}

void func1(uint8_t i) {
  Serial.print("Key Pressed:");
  Serial.println(i + 1);
}

void func2(uint8_t i) {
  Serial.print("Key Released:");
  Serial.println(i + 1);

}

void loop() {

  tm.buttonsCallbackFunc(func2, func1);

}
