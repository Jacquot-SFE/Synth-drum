#include "karlsen-fast-ladder.h"

/*
  Hot off the presses...posted to MusicDSP on Feb 16, 2016.
  See the bottom post here:
  http://musicdsp.org/showArchiveComment.php?ArchiveID=240

//Coupled with oversampling and simple oscs you will probably get the best analog approximation.

//        // for nice low sat, or sharper type low deemphasis saturation, one can use a onepole shelf before the filter.
//        b_lf = b_lf + ((-b_lf + b_v) * b_lfcut); // b_lfcut 0..1
//        double b_lfhp = b_v - b_lf;
//        b_v = b_lf + (b_lf1hp * ((b_lfgain*0.5)+1));   

        double b_rez = b_aflt4 - b_v; // no attenuation with rez, makes a stabler filter.
        b_v = b_v - (b_rez*b_fres); // b_fres = resonance amount. 0..4 typical "to selfoscillation", 0.6 covers a more saturated range.

        double b_vnc = b_v; // clip, and adding back some nonclipped, to get a dynamic like analog.
        if (b_v > 1) {b_v = 1;} else if (b_v < -1) {b_v = -1;}
        b_v = b_vnc + ((-b_vnc + b_v) * 0.9840);

        b_aflt1 = b_aflt1 + ((-b_aflt1 + b_v) * b_fenv); // straightforward 4 pole filter, (4 normalized feedback paths in series)
        b_aflt2 = b_aflt2 + ((-b_aflt2 + b_aflt1) * b_fenv);
        b_aflt3 = b_aflt3 + ((-b_aflt3 + b_aflt2) * b_fenv);
        b_aflt4 = b_aflt4 + ((-b_aflt4 + b_aflt3) * b_fenv);
        b_v = b_aflt4;

// Behave.
// Ove Hy Karlsen.
*/

extern int32_t paramf, paramq;

void AudioFilterKarlsen::update(void)
{
  audio_block_t *block;
  int16_t *p, *end;

  int16_t sample;

  int32_t b_v; // input value??
  int32_t b_vnc;
  int32_t b_rez, b_fres;
  int32_t b_fenv; // cutoff?
  int32_t temp;

  block = receiveWritable();
  if (!block) return;

  p = (block->data);
  end = p + AUDIO_BLOCK_SAMPLES;

  b_fres = resonance << 16;//0; 
  b_fenv = frequency<<16;//0x08000000 ;

  paramf = b_fenv;
  paramq = b_fres;
  
  
  while (p < end)
  {
    sample = *p;

    // Trying to come to term with the variable names in the original.
    //-------------------------
    // The "b_" are just prefixes for variables...a BEOS convention?
    // b_fres is the resonance parameter (0..4)
    // b_fenv is the cutoff frequency parameter (0..1).
    // b_v is the input value
    // b_vnc is input, unclipped.
    // b_aflt1..4 are the filter stage outputs.

    // turning signed q1.15 into q3.29: 
    b_v = sample << 14;
    
    //double b_rez = b_aflt4 - b_v; // no attenuation with rez, makes a stabler filter.
    b_rez = b_aflt4 - b_v;
    
    //b_v = b_v - (b_rez*b_fres); // b_fres = resonance amount. 0..4 typical "to selfoscillation", 0.6 covers a more saturated range.
    //b_v = b_v - multiply_32x32_rshift32(b_rez, b_fres);
    b_v = b_v - (multiply_32x32_rshift32(b_rez, b_fres)<<3);

    //double b_vnc = b_v; // clip, and adding back some nonclipped, to get a dynamic like analog.
    b_vnc = b_v;
#if 0
    //if (b_v > 1) {b_v = 1;} else if (b_v < -1) {b_v = -1;}
    // fixed pt impl is using q3.29 to clip at 0x2000.0000 and 0xe000.0000
    if(b_v >= 0x20000000)
    {
      b_v = 0x1fffffff;
    }
    else if (b_v <= 0xe0000000)
    {
      b_v = 0xe0000001;
    }
    //b_v = b_vnc + ((-b_vnc + b_v) * 0.9840);
    //                                         
    temp = (multiply_32x32_rshift32((b_v - b_vnc), 0x7df3b644 );
    b_v = b_vnc + temp;
#endif

    //b_aflt1 = b_aflt1 + ((-b_aflt1 + b_v) * b_fenv); // straightforward 4 pole filter, (4 normalized feedback paths in series)
    //  --------3------     ------1-------    --2----
    temp = b_v - b_aflt1;
    temp = multiply_32x32_rshift32(temp, b_fenv);
    b_aflt1 = b_aflt1 + temp;    
    
    //b_aflt2 = b_aflt2 + ((-b_aflt2 + b_aflt1) * b_fenv);
    temp = b_aflt1 - b_aflt2;
    temp = multiply_32x32_rshift32(temp, b_fenv);
    b_aflt2 = b_aflt2 + temp;    

    //b_aflt3 = b_aflt3 + ((-b_aflt3 + b_aflt2) * b_fenv);
    temp = b_aflt2 - b_aflt3;
    temp = multiply_32x32_rshift32(temp, b_fenv);
    b_aflt3 = b_aflt3 + temp;    

    //b_aflt4 = b_aflt4 + ((-b_aflt4 + b_aflt3) * b_fenv);
    temp = b_aflt3 - b_aflt4;
    temp = multiply_32x32_rshift32(temp, b_fenv);
    b_aflt4 = b_aflt4 + temp;    

    //b_v = b_aflt4;
    *p = b_aflt4 >> 14;
    p++;
  }

  transmit(block);
  release(block);

}


