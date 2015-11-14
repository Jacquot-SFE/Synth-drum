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

#include "Synth-DrumHeart.h"


// waveforms.c
extern "C" {
extern const int16_t AudioWaveformSine[257];
}


static bool trap;

void AudioSynthDrumHeart::noteOn(void)
{
  __disable_irq();

  //if(env_lin_current < 0x0ffff)
  {
    wav_phasor = 0;
  }

  env_lin_current = 0x7fff0000;
  
  trap = false;
  
  __enable_irq();
}

void AudioSynthDrumHeart::update(void)
{
  audio_block_t *block_wav, *block_env;
  int16_t *p_wave, *p_env, *end;
  int32_t sin_l, sin_r, interp, mod, mod2;
  uint32_t index, scale;


  block_env = allocate();
  if (!block_env) return;
  p_env = (block_env->data);
  end = p_env + AUDIO_BLOCK_SAMPLES;

  block_wav = allocate();
  if (!block_wav) return;
  p_wave = (block_wav->data);

  while(p_env < end)
  {
    // Do envelope first
    if(env_lin_current < 0x0000ffff)
    {
      *p_env = 0;
    }
    else
    {
      env_lin_current -= env_decrement;
      env_sqr_current = multiply_16tx16t(env_lin_current, env_lin_current) ;
      *p_env = env_sqr_current>>15;
    }  

    // do wave second;
    wav_phasor += wav_increment;

    // modulation changes how we use the increment
    // the increment will be scaled by the modulation amount.

    // Don't put data in the sign bits unless you mean it!
    mod = multiply_16tx16b((env_sqr_current), (wav_pitch_mod<< 5));// >> 16;
    mod2 = multiply_32x32_rshift32(wav_increment<< 4, mod<<1);
#if 0    
    if(!trap)
    {
      Serial.print(wav_increment, HEX);
      Serial.print(' ');
      Serial.print(mod, HEX);
      Serial.print(' ');
      Serial.println(mod2, HEX);
      trap = true;
    }
#endif
    
    wav_phasor += (mod2);
    wav_phasor &= 0x7fffffff;

    // then interp'd waveshape
    index = wav_phasor >> 23; // take top valid 8 bits
    sin_l = AudioWaveformSine[index];
    sin_r = AudioWaveformSine[index+1];

    scale = (wav_phasor >> 7) & 0xFFFF;
    sin_r *= scale;
    sin_l *= 0xFFFF - scale;
    interp = (sin_l + sin_r) >> 16;
    *p_wave = interp;

    p_env++;
    p_wave++;
 }

  transmit(block_env, 0);
  release(block_env);
  
  transmit(block_wav, 1);
  release(block_wav);

}

