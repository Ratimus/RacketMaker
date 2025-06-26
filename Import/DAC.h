#pragma once

#include <Arduino.h>

// DAC uses 2.048v vref and is amplified 3x ie 6.144V full scale
// the DACs/amps are not perfect and really should be calibrated in software
// 6v is 6 octaves so each note step is 4000/(6*12)=55.5555 counts. if we use 56 there is some error but its OK for a test
#define DAC_BITS 12
#define DAC_RANGE 4096
#define NOTE_STEP 55.5555
#define LOWEST_NOTE 24 // make C1 the lowest MIDI note ie 0V output
#define HIGHEST_NOTE 96 // highest MIDI note C7 ie 6V output

// void CVout(int channel, unsigned value) {
//   if (channel &2) digitalWrite(DAC1_CS, LOW); // assert correct chip select
//   else digitalWrite(DAC0_CS, LOW);
//   byte data = value >> 8;  // get to 8 bits of DAC value
//   data = data & B00001111; // mask off 4 msbs which are control
//   if (channel &1) data = data | B10110000; //2nd DAC channel, 2.048v mode
//   else data = data | B00110000; //1st DAC channel, 2.048v mode
//   SPI.transfer(data); // send  control bits and 4 MSBs of DAC value
//   data = value;
//   SPI.transfer(data);  // send low 8 bits of DAC value
//   if (channel &2) digitalWrite(DAC1_CS, HIGH);
//   else digitalWrite(DAC0_CS, HIGH);
// }
/*
float sysSampleRate;
float sysCallbackRate;
// static metronome_t tick;
float bufferIndex;
int32_t bufferIndex_int;
float bufferIndex_frac;
float pitchFactor;
float rootNotePitch;
float buffer[MAX_BUFFER_SIZE];  // sdram

static void AudioCallback(float *in, float *out, size_t size)
{
  float a, b;
  float sOut;

  for (size_t idx(0); idx < size; ++idx)
  {
    if (tick.Process())  // sIndex has been sset to 0
    {
      ;
    }

    bufferIndex += pitchFactor;
    bufferIndex_int = static_cast<int32_t>(bufferIndex);
    bufferIndex_frac = bufferIndex - bufferIndex_int;
    a = buffer[bufferIndex_int];
    b = buffer[bufferIndex_int + 1];
    sOut = a + (b - a) * bufferIndex_frac;
    if (bufferIndex > (4800 * 10))
    {
      bufferIndex = 0;
    }
    out[idx] = sOut;
    out[idx + 1] = sOut;
  }

}

playbackSpeed = (speedCV * (SPEED_LOG_MAX - SPEED_LOG_MIN) + SPEED_LOG_MIN)) * SPEED_SCALAR;
// 0 <= speedCV <= 1
// SPEED_LOG_MIN = 0; // log(1)
// SPEED_LOG_MAX = log(2^5); // 5 octave range
// SPEED_SCALAR = 0.125; // (2^3), to shift down 3 octaves
put all this in a table with 256 step per semitone resolution (store as reciprocals if you want to save some math)
for each item in the table, multiply by 2^1/12 * 256
for a midi note number, shift it << 8 and index the table with the result - that's your frequency (or reciprocal)
if sample is tuned to e.g. 440 Hz at 16 KHz sample frequency and you want to play one octave above, take the value
of the table at 880 Hz, multiply by (16000 / 440), and that's the 32 KHz frequency
*/



