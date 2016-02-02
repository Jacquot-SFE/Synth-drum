/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef effect_exponential_decay_h_
#define effect_exponential_decay_h_
#include "AudioStream.h"
#include "utility/dspinst.h"

//#define SAMPLES_PER_MSEC (AUDIO_SAMPLE_RATE_EXACT/1000.0)

class AudioEffectClapEnvelope : public AudioStream
{
public:
  AudioEffectClapEnvelope() : AudioStream(1, inputQueueArray) {
    length(1000);
  }
  void noteOn();

  void length(int32_t milliseconds)
  {
    if(milliseconds < 0)
      return;
    if(milliseconds > 5000)
      milliseconds = 5000;

    int32_t len_samples = milliseconds*(AUDIO_SAMPLE_RATE_EXACT/1000.0);


    __disable_irq();    

    increment = (0x7fff0000/len_samples);
    
    __enable_irq();    
  };
  
  using AudioStream::release;
  virtual void update(void);

  // public for debug...
  int32_t current; // present value
  uint32_t state;
  int32_t increment; // (actually, decrement, how each sample deviates from previous.)

private:
  audio_block_t *inputQueueArray[1];

  //int32_t current; // present value
  //int32_t increment; // (actually, decrement, how each sample deviates from previous.)

};

#undef SAMPLES_PER_MSEC
#endif

