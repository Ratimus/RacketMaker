
// Copyright 2023 Ryan Richardson
//
// Author: Ratimus
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
#pragma once

#include <Arduino.h>
#include <atomic>
#include "Latchable.h"
#include "constants.h"

// Available sample sets:
//   -DSAMPLEDEFS_909   (TR-909)
//   -DSAMPLEDEFS_808   (TR-808)
//   -DSAMPLEDEFS_WSA   (WSA Soundfont)
//   -DSAMPLEDEFS_AJS   (Jungle Set)
//   -DSAMPLEDEFS_ATS   (Techno Set)

#if defined(SAMPLEDEFS_909)
#include <sampledefs_909.h>
#elif defined(SAMPLEDEFS_808)
#include <sampledefs_808.h>
#elif defined(SAMPLEDEFS_WSA)
#include <sampledefs_wsa.h>
#elif defined(SAMPLEDEFS_AJS)
#include <sampledefs_AJS.h>
#elif defined(SAMPLEDEFS_ATS)
#include <sampledefs_ATS.h>
#else
#error "No sampledefs set! Define SAMPLEDEFS_xxx in platformio.ini"
#endif

enum CV_MODE
{
  NONE = 0,
  VOLUME,
  SAMPLE,
  DECAY,
  PITCH
};

class Voice
{
  std::atomic<bool> running = false;

public:
  Voice()
  {
    pSample = &SAMPLE_BANK[sample.out];
  }

  float getSample()
  {
    // Interpolate between the current sample and the next one as a function of hypothetical
    // sample rate necessary to play at a given pitch versus the current sample rate (yeah,
    // I get that makes no sense; just read the code)
    uint32_t sampleindex_int = static_cast<int32_t>(sampleindex);
    float sampleindex_frac = sampleindex - sampleindex_int;

    int16_t sampleA(pSample->samplearray[sampleindex_int]);
    int16_t sampleB(pSample->samplearray[sampleindex_int + 1]);

    sampleindex += sampleindexFactor;

    // Get the sample val, and the interpolation
    return float((float)sampleA + (float)(sampleB - sampleA) * sampleindex_frac);
  }

  void overdriveProcess(float *bvf)
  {
    // Overdrive algorithm:
    // y = x(1 - e^-|x|) / |x|
    float fac(32768.0);
    *bvf /= fac;
    if (*bvf > 0.0)
    {
      *bvf = 1 - exp(-*bvf);
    }
    else
    {
      *bvf = -1 + exp(*bvf);
    }

    *bvf *= fac;
  }

  uint16_t scaleOutput(float *bvf)
  {
    int32_t buffVal(*bvf);
    if (buffVal < -32768)
    {
      buffVal = -32768 + (buffVal + 32768);
    }
    else if (buffVal > 32767)
    {
      buffVal = 32767 - (buffVal - 32767);
    }
    buffVal += 32768;

    return uint16_t(buffVal >> 4);
  }

  void tick(uint16_t *pVal)
  {
    // If we've played the whole sample, reset it and bail.
    if ((sampleindex >= pSample->samplesize) || (sampleindex < 0.0f) || (nextEnvFactor <= 0.0001f))
    {
      *pVal = 2048;
      return;
    }

    // Get the sample val, add the interpolation
    float bvf(getSample());
    bvf *= level;

    bvf *= nextEnvFactor;
    nextEnvFactor *= envelopeScaler;

    // Apply soft clipping
    overdriveProcess(&bvf);

    // Convert our value from 16 bits signed to 12 bits unsigned
    *pVal = scaleOutput(&bvf);
  }

  bool started()
  {
    return running.exchange(false, std::memory_order_relaxed);
  }

  // Starts playing a sample
  bool start(uint16_t CV_in)
  {
    running.store(true, std::memory_order_relaxed);
    level = muted.out ? 0.0 : (float)mix.clock();
    shape = envShape.clock();
    envDecay = oneOverSampleRate / (static_cast<float>(decay.clock() + 1.0f) / 1000.0f);
    pitchScalar = (float)pitch.clock();
    sampleOffset = sample.clock();

    int32_t cvSquared;
    int32_t cv32;

    float cvFloat(CV_in);
    float cvScalar;

    // Do some modulation stuff
    switch (cv_mode.clock())
    {
    case VOLUME:
      cv32 = (int32_t)CV_in - 2048;
      cvSquared = cv32 * cv32;
      if (cv32 < 0)
      {
        cvSquared *= -1;
      }

      cvScalar = ((float)cvSquared / (float)0x3FF001); // 0x3FF001 == 2047 * 2047
      level += 100.0f * cvScalar;
      if (level < 0.0)
      {
        level = 0.0;
      }
      break;

    case SAMPLE:
      sampleOffset += CV_in * NUM_SAMPLES / 4095;
      if (sampleOffset >= NUM_SAMPLES)
      {
        sampleOffset -= NUM_SAMPLES;
      }
      break;

    case DECAY:
      envDecay += 2047.0f / (cvFloat - 2048.0f);
      break;

    case PITCH:
      cv32 = (int32_t)CV_in - 2048;
      cv32 *= 96;
      pitchScalar += (float)cv32 / 4095.0f;
      break;

    case NONE:
      break;

    default:
      cv_mode.clock_in(NONE); // if its messed up fix it
      break;
    }

    pSample = &SAMPLE_BANK[sampleOffset];
    pitchScalar = pow(2.0f, pitchScalar / 96.0f);
    sampleindex = 0.0f;
    sampleindexFactor = pitchScalar;

    level /= 100.0f;
    envelopeScaler = powf(0.0001f, envDecay);
    nextEnvFactor = 1.0f;

    return choke.clock();
  }

  void stop()
  {
    sampleindex = -1;
    running.store(false, std::memory_order_relaxed);
  }

  void setDefaults(
      uint8_t sp,
      uint8_t mx,
      uint8_t dc,
      int8_t pt,
      uint8_t cv,
      uint8_t shape,
      bool ch)
  {
    sample.jam(sp);
    mix.jam(mx);
    decay.jam(dc);
    pitch.jam(pt);
    cv_mode.jam(cv);
    envShape.jam(shape);
    choke.jam(ch);
    resetToDefaults();
  }

  void resetToDefaults()
  {
    sample.reset();
    mix.reset();
    decay.reset();
    pitch.reset();
    cv_mode.reset();
    envShape.reset();
    choke.reset();
    pSample = &SAMPLE_BANK[sample.out];
  }

  latchable<bool> muted = false;
  latchable<bool> choke = false;
  latchable<uint8_t> mix = 100;
  latchable<int8_t> pitch = 0;
  latchable<uint16_t> decay = 1000;
  latchable<uint8_t> sample = 0;
  latchable<uint8_t> cv_mode = 0;
  latchable<uint8_t> envShape = 0;
  float level = 1.0;
  float shape = 0;
  float envDecay = 1.0;
  int32_t buffVal = 0;
  float pitchScalar = 0;
  float sampleindex = -1.0;
  const sample_t *pSample = nullptr;
  uint8_t sampleOffset = 0;
  float sampleindexFactor = 0;
  float nextEnvFactor = 0;
  float envelopeScaler = 0;
  inline static constexpr float oneOverSampleRate = (1.0f / 22000.0f);
};

extern Voice voice[4];
