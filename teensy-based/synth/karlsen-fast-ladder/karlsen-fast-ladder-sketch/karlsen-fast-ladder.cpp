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
  audio_block_t *audio_block;
  audio_block_t *control_block;
  int16_t *p, *ctl, *end;

  int16_t sample;

  int32_t b_v; // input value??
  int32_t b_vnc;
  int32_t b_rez, b_fres;
  int32_t b_fenv; // cutoff?
  int32_t temp;

  // And values from the state variable control input...
  int32_t control;
  int32_t fmult, n;

  audio_block = receiveWritable(0);
  if (!audio_block) return;

  control_block = receiveReadOnly(1);
  if (!control_block) return;

  p = (audio_block->data);
  end = p + AUDIO_BLOCK_SAMPLES;

  ctl = (control_block->data);

  b_fres = resonance << 16;//0; 
  b_fenv = frequency<<15;//0x08000000 ;

  paramf = b_fenv;
  paramq = b_fres;
  
  
  while (p < end)
  {
#if 0
    // Trying to implement frequency control via audio pipeline.
    control = *ctl++;
    control *= setting_octavemult;
    n = control &0x7ffffff; // 27 fractional bits.

    // exp2 algorithm by Laurent de Soras
    // http://www.musicdsp.org/showone.php?id=106
    n = (n + 134217728) << 3;
    n = multiply_32x32_rshift32_rounded(n, n);
    n = multiply_32x32_rshift32_rounded(n, 715827883) << 3;
    n = n + 715827882;
    //#endif
    n = n >> (6 - (control >> 27)); // 4 integer control bits
    //ult = multiply_32x32_rshift32_rounded(fcenter, n);
    fmult = multiply_32x32_rshift32_rounded(frequency, n);
    if (fmult > 5378279) fmult = 5378279;
    fmult = fmult << 8;
    b_fenv = fmult;
#else
    b_fenv = *ctl++;
    b_fenv <<= 15;   
    b_fenv += 0x40000000; 
#endif

    
    
    sample = *p;
    // Trying to come to term with the variable names in the original.
    // I think some of the names get recycled as temps for calculations
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
#if 1
    //if (b_v > 1) {b_v = 1;} else if (b_v < -1) {b_v = -1;}
    // fixed pt impl is using q3.29 to clip at 0x2000.0000 and 0xe000.0000
    if(b_v >= 0x1000000)
    {
      //Serial.print(".");
      b_v = 0x0ffffff;
    }
    else if (b_v <= -0x1000000)//0xe0000000)
    {
      // yes, this is intentionally asymmetrical...
      // it seemed to sound ok?
      
      //b_v = 0xe0000001;
      //Serial.print("-");
      b_v = -0x0ffffff;
    }
    //b_v = b_vnc + ((-b_vnc + b_v) * 0.9840);
    //                                         
    temp = multiply_32x32_rshift32((b_v - b_vnc), 0x7df3b644 );
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

  transmit(audio_block);
  release(audio_block);
  release(control_block);

}


