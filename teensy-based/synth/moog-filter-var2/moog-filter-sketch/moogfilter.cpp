#include "moogfilter.h"

// pulled from here:
// http://musicdsp.org/showArchiveComment.php?ArchiveID=26
// and ported to 16-bit fixedpoint
//
//
/*
 *
fc = cutoff, nearly linear [0,1] -> [0, fs/2]
res = resonance [0, 4] -> [no resonance, self-oscillation]

in[x] and out[x] are member variables, init to 0.0 the controls:


Tdouble MoogVCF::run(double input, double fc, double res)
{
a: double f = fc * 1.16;
b: double fb = res * (1.0 - 0.15 * f * f);
c: input -= out4 * fb;
d: input *= 0.35013 * (f*f)*(f*f);
e: out1 = input + 0.3 * in1 + (1 - f) * out1; // Pole 1
f: in1 = input;
g: out2 = out1 + 0.3 * in2 + (1 - f) * out2; // Pole 2
h: in2 = out1;
i: out3 = out2 + 0.3 * in3 + (1 - f) * out3; // Pole 3
j: in3 = out2;
k: out4 = out3 + 0.3 * in4 + (1 - f) * out4; // Pole 4
l: in4 = out3;
m: return out4;
}
*/


void AudioFilterMoog::update(void)
{
  audio_block_t *block;
  int16_t *p, *end;
  int16_t input, temp;

  uint16_t f, f4;
  int16_t fb;

  block = receiveWritable();
  if (!block) return;

  p = (block->data);
  end = p + AUDIO_BLOCK_SAMPLES;

  // try frame-by-frame cutoff and Q to start...
  //a: double f = fc * 1.16;
  f = fc;//(fc * fc) >> 15;
#if 0
  f += (fc * 0x??28f5) >> 16;
#endif

  //b: double fb = res * (1.0 - 0.15 * f * f);
#if 0
  fb = 0x01;
#else
  temp = (f * f) >> 15;
  temp = 0x7fff - ((temp * 0x2666) >> 16);
  // Res is Q3.13
  // Let fb be 3.13 also
  fb = (res * temp) >> 15;
  //fb = (res * temp) >> 12;
#endif

#if 0
  Serial.print(fc);
  Serial.print(' ');
  Serial.print(f);
  Serial.print(' ');
  Serial.println(fb);
#endif

  while (p < end)
  {
    input = *p;
#if 1
    // c: input -= out4 * fb;
    // feedback is q3.13, signed
    // IE can be multiply by up to 4-lsb.
    input -= (out4 * fb) >> 13;

    // d: input *= 0.35013 * (f*f)*(f*f);
    input = (input * 0x59a2) >> 16;
    f4 = (f * f) >> 16;
    f4 = (f4 * f4) >> 16;
    input = (input * f4 ) >> 16;

    // e: out1 = input + 0.3 * in1 + (1 - f) * out1; // Pole 1
    //out1 = input + 0x2666 * in1 + (0x8000 - f) * out1;
    //       -----   ------------   -------------------
    temp = ((0xffff - f) * out1) >> 16;
    out1 = (0x4ccc * in1) >> 16;
    out1 += input + temp;

    // f: in1 = input;
    in1 = input;
    // g: out2 = out1 + 0.3 * in2 + (1 - f) * out2; // Pole 2
    temp = ((0xffff - f) * out2) >> 16;
    out2 = (0x4ccc * in2) >> 16;
    out2 += out1 + temp;

    // h: in2 = out1;
    in2 = out1;

    // i: out3 = out2 + 0.3 * in3 + (1 - f) * out3; // Pole 3
    temp = ((0xffff - f) * out3) >> 16;
    out3 = (0x4ccc * in3) >> 16;
    out3 += out2 + temp;

    // j: in3 = out2;
    in3 = out2;

    // k: out4 = out3 + 0.3 * in4 + (1 - f) * out4; // Pole 4
    temp = ((0xffff - f) * out4) >> 16;
    out4 = (0x4ccc * in4) >> 16;
    out4 += out3 + temp;

    // l: in4 = out3;
    in4 = out3;

    // m: return out4;
    *p = out4;

#else
    *p = input;
#endif
    p++;
  }

  transmit(block);
  release(block);

}


