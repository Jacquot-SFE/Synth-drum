#include "moogfilter.h"

/*
The MusicDSP list "Moog VCF, variation #1" filter


// Moog 24 dB/oct resonant lowpass VCF
// References: CSound source code, Stilson/Smith CCRMA paper.
// Modified by paul.kellett@maxim.abel.co.uk July 2000

float f, p, q; //filter coefficients
float b0, b1, b2, b3, b4; //filter buffers (beware denormals!)
float t1, t2; //temporary buffers

// Set coefficients given frequency & resonance [0.0...1.0]

q = 1.0f - frequency;
p = frequency + 0.8f * frequency * q;
f = p + p - 1.0f;
q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));

// Filter (in [-1.0...+1.0])

in -= q * b4; //feedback
t1 = b1; b1 = (in + b0) * p - b1 * f;
t2 = b2; b2 = (b1 + t1) * p - b2 * f;
t1 = b3; b3 = (b2 + t2) * p - b3 * f;
b4 = (b3 + t1) * p - b4 * f;
b4 = b4 - b4 * b4 * b4 * 0.166667f; //clipping
b0 = in;

// Lowpass output: b4
// Highpass output: in - b4;
// Bandpass output: 3.0f * (b3 - b4);

*/

extern int16_t qpeek, rezpeek;

void AudioFilterMoog2::update(void)
{
  audio_block_t *block;
  int16_t *p, *end;

  int16_t in, temp, t1, t2;
  float qf, femp;

  block = receiveWritable();
  if (!block) return;

  p = (block->data);
  end = p + AUDIO_BLOCK_SAMPLES;

  // a: q = 1.0f - frequency;
  _q = 0x7fff - frequency;

  // b: p = frequency + 0.8f * frequency * q;
  temp = (multiply_16bx16b(0x6666, frequency)) >> 15;
  temp = (multiply_16bx16b(temp, _q)) >> 15;
  _p = frequency + temp;

  // c: f = p + p - 1.0f;
  _f = _p + _p - 0x7fff;

  //d: q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));
  //       -----7---    --6--  --5-  -4-   ----3---   --2--  --1--
#if 0
  qf = (float)_q/0x7fff;
  femp = (1.0f + 0.5f * qf * (1.0f - qf + 5.6f * qf * qf));
  //Serial.println(femp);
  temp = (int16_t)(femp  * 0x7fff);
  temp &= 0x7fff;
  _q = (multiply_16bx16b(resonance, temp)) >> 15;
#else
  temp = (multiply_16bx16b(_q, _q)) >> 15;
  temp = (multiply_16bx16b(temp, 0x2cccc)) >> 15;
  temp = 0x7fff - _q - temp;

  temp = (multiply_16bx16b(temp, _q)) >> 15;
  temp >>= 1;
  temp = 0x7fff - temp;

  _q = (multiply_16bx16b(resonance, temp)) >> 15;
  //_q = 0x4000;
#endif
  rezpeek = resonance;
  qpeek = _q;

  while (p < end)
  {
#if 1
    in = *p;
    //in -= q * b4; //feedback
    temp = (multiply_16bx16b(_q, _b4)) >> 14;
    in = in - temp;

    //t1 = b1;
    t1 = _b1;

    //b1 = (in + b0) * p - b1 * f;
    temp = (multiply_16bx16b((in + _b0), _p)) >> 15;
    _b1 = temp - ((multiply_16bx16b(_b1, _f)) >> 15);

    //t2 = b2;
    t2 = _b2;
    
    //b2 = (b1 + t1) * p - b2 * f;
    temp = (multiply_16bx16b((_b1 + t1), _p)) >> 15;
    _b2 =  temp -(multiply_16bx16b(_b2, _f) >> 15);
    
    //t1 = b3;
    t1 = _b3;
    
    //b3 = (b2 + t2) * p - b3 * f;
    temp = (multiply_16bx16b((_b2 + t2), _p)) >> 15;
    _b3 =  temp -((multiply_16bx16b(_b3, _f)) >> 15);
    
    //b4 = (b3 + t1) * p - b4 * f;
    temp = (multiply_16bx16b((_b3 + t1), _p)) >>15;
    _b4 = temp - ((multiply_16bx16b(_b4, _f))>>15);

    //b4 = b4 - b4 * b4 * b4 * 0.166667f; //clipping
    temp = (multiply_16bx16b(_b4, _b4))>>15;
    temp = (multiply_16bx16b(temp, _b4))>> 15;
    temp = (multiply_16bx16b (temp, 0x1555))>>15;
    _b4 -= temp;
    
    //b0 = in;
    _b0 = in;

    *p = _b4;

#else
    *p = input;
#endif
    p++;
  }

  transmit(block);
  release(block);

}


