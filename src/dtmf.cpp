

#include "Arduino.h"
#include "dtmf.h"
#include "Dac.h"

#define ROW_697   14 // 697/50=13.97, get 700Hz
#define ROW_770   15 // 770/50=15.4, get 750Hz
#define ROW_852   17 // 850 Hz
#define ROW_941   19 // 950 Hz

#define COL_1209  24 //1200 Hz
#define COL_1336  27 //1350 Hz
#define COL_1477  30 //1500 Hz
#define COL_1633  33 //1650 Hz

#define WAVE_TABLE_SIZE 200

//audio shield dac with MCP4921
Dac dac;

static DTMF *generatorInstance = NULL;

unsigned char DTMF::dtmfRowPos = 0;
unsigned char DTMF::dtmfColPos = 0;
unsigned char DTMF::rowOffset = 0;
unsigned char DTMF::colOffset = 0;

unsigned char waveTable[WAVE_TABLE_SIZE];
// we clock through the wave table at 10Khz.
// off-set is our increment through the wave table.
// There are 200 samples in the table.
// 10,000/200 = 50 Hz, so f/50 = table increment.
// E.g. 697 Hz / 50 = 13.94, so round to 14.
DTMF::DTMF() {
  // Update global object with current class instance.
  generatorInstance = this;

  // Setup global variables with initial values.
  dtmfRowPos = 0;
  dtmfColPos = 0;

  // generate a sine wave in the waveTable
  // The wavetable will be a set of numbers that range from
  // 0..126
  for (int i = 0; i < WAVE_TABLE_SIZE; i++) {
    // table ranges from 0..127
    // theta ranges from 0..2*PI.
    float theta = PI * i / 100; 
    waveTable[i] = round(63 * (1 + sin(theta)));
  }
}
// write a method that plays the wave table, for testing
// purposes.
// Use
/*for (int i=0; i < 200; i++){
    byte output = waveTable[i];
    dac.dacWrite(output);
   }
 */
void DTMF::generate(char key, unsigned long duration) {
  unsigned long timerCycle = 0;

  // This library allows numerical characters, # and * only.
  if (((key >= '0') && (key <= '9')) ||
      ((key == '#') ||  (key == '*')) ||
      ((key >= 'A') && (key <= 'D'))) {
    // Distinguish the low-frequency
    // and high-frequency components of the DTMF waveform.
    switch (key) {
      case '1':
        rowOffset = ROW_697;
        colOffset = COL_1209;
        break;
      case '2':
        rowOffset = ROW_697;
        colOffset = COL_1336;
        break;
      case '3':
        rowOffset = ROW_697;
        colOffset = COL_1477;
        break;
      case '4':
        rowOffset = ROW_770;
        colOffset = COL_1209;
        break;
      case '5':
        rowOffset = ROW_770;
        colOffset = COL_1336;
        break;
      case '6':
        rowOffset = ROW_770;
        colOffset = COL_1477;
        break;
      case '7':
        rowOffset = ROW_852;
        colOffset = COL_1209;
        break;
      case '8':
        rowOffset = ROW_852;
        colOffset = COL_1336;
        break;
      case '9':
        rowOffset = ROW_852;
        colOffset = COL_1477;
        break;
      case '*':
        rowOffset = ROW_941;
        colOffset = COL_1209;
        break;
      case '0':
        rowOffset = ROW_941;
        colOffset = COL_1336;
        break;
      case '#':
        rowOffset = ROW_941;
        colOffset = COL_1477;
        break;
      case 'A':
        rowOffset = ROW_697;
        colOffset = COL_1633;
        break;
      case 'B':
        rowOffset = ROW_770;
        colOffset = COL_1633;
        break;
      case 'C':
        rowOffset = ROW_852;
        colOffset = COL_1633;
        break;
      case 'D':
        rowOffset = ROW_941;
        colOffset = COL_1633;
        break;
    }

    // Start TIMER1 with frequency of 10KHz.
    noInterrupts();

    // Initialize TIMER1 registers to known state.
    initTimers0();
    // Set TIMER1 to 10KHz with reference to 16MHz Arduino clock.
    OCR1A = 24;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS11) | (1 << CS10);

    // Enable TIMER1 interrupt.
    TIMSK1 |= (1 << OCIE1A);
    interrupts();
    // Wait for specified duration.
    while (timerCycle < duration) {
      timerCycle++;
      delay(1);
    }
    disableTimer1();
    byte output = waveTable[0] + waveTable[0];
    dac.dacWrite(output);
  }
}
void DTMF::initTimers0() {
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TIMSK1 = 0;
}
void DTMF::disableTimer1() {
  // Disable TIMER1 interrupt and clear all TIMER1 associated registers.
  noInterrupts();
  initTimers0();
  interrupts();
}

void DTMF::waveGenerator() {
  // Generate output waveform by mixing low-frequency and high-frequency waveforms.
  int output = waveTable[dtmfRowPos] + waveTable[dtmfColPos];
  //
  // 126+126 = 252 gives you a little headroom relative the 255 full value of a byte.
  //
  dac.dacWrite(output);

  dtmfRowPos += rowOffset;
  dtmfColPos += colOffset;

  // Check for end of low-frequency wave cycle.
  if (dtmfRowPos >= WAVE_TABLE_SIZE) {
    dtmfRowPos -= WAVE_TABLE_SIZE;
  }

  // Check for end of high-frequency wave cycle.
  if (dtmfColPos >= WAVE_TABLE_SIZE)  {
    dtmfColPos -= WAVE_TABLE_SIZE;
  }
}

void DTMF::generateOutput() {
  if (generatorInstance != NULL)  {
    generatorInstance->waveGenerator();
  }
}

ISR(TIMER1_COMPA_vect) {
  DTMF::generateOutput();
}
