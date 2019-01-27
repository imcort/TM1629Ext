#include "Arduino.h"
#include "TM1629Ext.h"


// uint8_t ReverseBits(uint8_t ch){
// ch = (ch & 0x55) << 1 | (ch >> 1) & 0x55;
// ch = (ch & 0x33) << 2 | (ch >> 2) & 0x33;
// ch = (ch & 0x0F) << 4 | (ch >> 4) & 0x0F;
// uint8_t chn = 0x00;
// chn |= ((ch & 0b10000000) >> 7)&0xff;
// chn |= ((ch & 0b01000000) >> 4)&0xff;
// chn |= ((ch & 0b00100000) << 1)&0xff;
// chn |= ((ch & 0b00010000) << 1)&0xff;
// chn |= ((ch & 0b00001000) << 1)&0xff;
// chn |= ((ch & 0b00000100) >> 1)&0xff;
// chn |= ((ch & 0b00000010) << 6)&0xff;
// chn |= ((ch & 0b00000001) << 3)&0xff;

// return chn;
// }

Tm1629Ext::Tm1629Ext(uint8_t strobe, uint8_t clock, uint8_t data) {
  
  STROBE_IO = strobe;
  DATA_IO = data;
  CLOCK_IO = clock;

}

void Tm1629Ext::begin(){

  expander.begin(0x20);

  expander.pinMode(STROBE_IO, OUTPUT);
  expander.pinMode(CLOCK_IO, OUTPUT);
  expander.pinMode(DATA_IO, OUTPUT);

  sendCommand(ACTIVATE);
  reset();

}

void Tm1629Ext::sendCommand(uint8_t value)
{
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, value);
  expander.digitalWrite(STROBE_IO, HIGH);
}

void Tm1629Ext::reset() {
  sendCommand(WRITE_INC); // set auto increment mode
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, 0xc0);   // set starting address to 0
  for (uint8_t i = 0; i < 16; i++)
  {
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, 0x00);
  }
  for (uint8_t i = 0; i < 16; i++)
  {
    displayCache[i] = 0x00;
  }
  expander.digitalWrite(STROBE_IO, HIGH);
  sendCommand(WRITE_INC); // set auto increment mode
  sendCommand(ACTIVATE);
}

uint32_t Tm1629Ext::readButtons()
{
  uint32_t buttons = 0;
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, BUTTONS);
  expander.pinMode(DATA_IO, INPUT);

  for (uint8_t i = 0; i < 4; i++)
  {
    uint8_t v = expander.shiftIn(DATA_IO, CLOCK_IO, LSBFIRST);
    buttons |= (v << (i*8));
  }

  expander.pinMode(DATA_IO, OUTPUT);
  expander.digitalWrite(STROBE_IO, HIGH);
  return buttons;
}

void Tm1629Ext::displayText(String text) {
  uint8_t length = text.length();
  if(length <= 16)
  {
    for (uint8_t i = 0; i < length; i++) {
        displayCache[i] = ss[text[i]];
        //displayASCII(i, text[i]);
    }
    updateDisplay();
  }
    
}

void Tm1629Ext::displaySS(uint8_t position, uint8_t value) { // call 7-segment
  displayCache[position] = value;//ReverseBits(value);
  updateDisplay();
}

void Tm1629Ext::updateDisplay(){   ///OWN

  sendCommand(WRITE_INC);
  expander.digitalWrite(STROBE_IO, LOW);
  expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, 0xc0);   // set starting address to 0
  for (uint8_t i = 0; i < 8; i++)
  {
    uint8_t tempData = 0x00;  //SEG1-8
    for (uint8_t j=0; j<8; j++){
      tempData |= (displayCache[15-j] & (0x80 >> i)) << i >> j;
    }
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, tempData);

    tempData = 0x00;  //SEG9-10
    for (uint8_t j=0; j<8; j++){
      tempData |= (displayCache[7-j] & (0x80 >> i)) << i >> j;
    }
    expander.shiftOut(DATA_IO, CLOCK_IO, LSBFIRST, tempData);

  }
  expander.digitalWrite(STROBE_IO, HIGH);
  sendCommand(ACTIVATE); // set auto increment mode
}

void Tm1629Ext::displayASCII(uint8_t position, uint8_t ascii) {
  displaySS(position, ss[ascii]);
}

void Tm1629Ext::displayHex(uint8_t position, uint8_t hex) {
  displaySS(position, hexss[hex]);
}

void Tm1629Ext::displayNumber(uint8_t position, int32_t num) {
  switch(position){
    case 0:
      if(num > 999)
        return;
      displayHex(0,num/100);
      displayHex(1,num/10%10);
      displayHex(2,num/1%10);
      break;
    case 1:
      if(num > 99999 || num < -9999)
        return;
      if(num < 0){
        displayASCII(3,'-');
        num = -num;
      } 
      else{
        displayASCII(3,num/10000%10);
      }
      displayHex(4,num/1000%10);
      displayHex(5,num/100%10);
      displayHex(6,num/10%10);
      displayHex(7,num/1%10);
      break;
    case 2:
      break;
  }
}
