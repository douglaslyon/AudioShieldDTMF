#ifndef DAC_H
#define DAC_H
#include "Arduino.h"
#define CS_ADC_BAR 10
#define AD_LATCH 6
class Dac {
  public:
    Dac();
    void dacWrite(byte);
    void dacWrite16(int data);
    
};

#endif
