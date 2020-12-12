#include "Arduino.h"
#include "Dac.h"
#include <SPI.h>
//CS  MCP4921 = pin 9 of the audio shield
//xFF00 or x00FF if shifting, is a signal
// that ranges from x0000 to xFF00 = 0..65,280
// whereas x0000...x00FF ranges from 0..255
// and xFFFF = 65535 (base 10) 65535 - 255 = 65,280.
//x00FFF = 4095 (base 10) for a 12 bit dac
Dac::Dac() {
  int CS = 9;
  int LDAC = 8;
  pinMode(CS, OUTPUT);
  pinMode(LDAC, OUTPUT);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  SPI.begin();
  //PORTB |= 0b00000011;
  digitalWrite(CS, HIGH);
  digitalWrite(LDAC, HIGH);
  //set chip select and LDAC to default high state
}
//this output byte will be the most significant
//8 bits of output on the dac
// output ranges from 0..255
void Dac::dacWrite(byte output) {
  uint16_t outbuf = 0;
  outbuf = output;                //DC bias can be added here by left shifting
  outbuf |= 0b0111000000000000;   //change some bits at the beginning to configure DAC
  char* w_bytes;
  w_bytes = (char*)&outbuf;       //split the two bytes in outbuf into an array
  PORTB &= 0b11111101;            //pull chip select low
  SPI.transfer( w_bytes [1] );    //send them one at a time
  SPI.transfer( w_bytes [0] );
  PORTB |= 0b00000010;            //pull chip select high
  PORTB &= 0b11111110;            //pull LDAC low
  asm volatile("nop");            //wait one clock cycle
  PORTB |= 0b00000001;            //pull LDAC high
}
void Dac::dacWrite16(int data) { // send data to the dac
  digitalWrite(CS_ADC_BAR, LOW); // replace by below
  //PORTB &= ~0x4;
  SPI.transfer(((data >> 8) & 0x0f) | 0x70);
  SPI.transfer(data & 0xff);
  digitalWrite(CS_ADC_BAR, HIGH); // replace by below
  //PORTB |= 0x4;
  digitalWrite(AD_LATCH, LOW); // replace by below
  //PORTD &= ~0x40;
  digitalWrite(AD_LATCH, HIGH); // replace by below
  //PORTD |= 0x40;
}
