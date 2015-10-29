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

#include "Effect-ExponentialDecay.h"


void AudioEffectExponentialDecay::noteOn(void)
{
  __disable_irq();

  mult = 0x10000;
  
  __enable_irq();
}

void AudioEffectExponentialDecay::update(void)
{
  audio_block_t *block;
  uint32_t *p, *end;
  //int16_t *p, *end;
  int16_t temp;
  //uint32_t sample12, sample34, sample56, sample78, tmp1, tmp2;
  uint32_t sample12, tmp1, tmp2;

  block = receiveWritable();
  if (!block) return;

  p = (uint32_t *)(block->data);
  end = p + AUDIO_BLOCK_SAMPLES/2;
  //p = block->data;
  //end = p + AUDIO_BLOCK_SAMPLES;

  // process all samples
  while (p < end) 
  {
    // try the naive implementation to begin...
    mult = ((uint32_t)mult * coeff) >> 16;
    //mult -= 100;

#if 0
    // 2.76%
    temp = *p;
    temp = ((temp * mult) >> 16) & 0x0ffff;
    *p = temp;
    p++;
#else
  //2.2%...
  // Borrowed from the envelope class
    sample12 = *p++;
    p -= 1;
    tmp1 = signed_multiply_32x16b(mult, sample12);
    //mult += inc;
    tmp2 = signed_multiply_32x16t(mult, sample12);
    sample12 = pack_16b_16b(tmp2, tmp1);
    *p++ = sample12;
#endif    
  }

  transmit(block);
  release(block);

}

