#ifndef _moogfilter_h_
#define _moogfilter_h_
#include "AudioStream.h"
#include "utility/dspinst.h"

// #define SAMPLES_PER_MSEC (AUDIO_SAMPLE_RATE_EXACT/1000.0)
// AUDIO_SAMPLE_RATE_EXACT
//    __disable_irq();    
//    __enable_irq();    

class AudioFilterKarlsen : public AudioStream
{
public:
  AudioFilterKarlsen() : AudioStream(2, inputQueueArray) 
  {
    b_aflt1 = b_aflt2 = b_aflt3 = b_aflt4 = 0;
    
    //frequency = 0x1000;
    //frequency = 0x0800;
    resonance = 0;
  };

  void cutoff(float val)
  {
#if  1
    frequency = val;
#else    
    // Again, borrowed from the SVF code.
    if (val < 20.0) 
      val = 20.0;
    else if (val > AUDIO_SAMPLE_RATE_EXACT/2.5) 
      val = AUDIO_SAMPLE_RATE_EXACT/2.5;
      
    /*setting_fcenter*/ frequency = (val * (3.141592654/(AUDIO_SAMPLE_RATE_EXACT*2.0)))
      * 2147483647.0;
#endif
  };
  void q(int16_t val)
  {
    resonance = val;
  };
    
  void octaveControl(float n) 
  {
    // filter's corner frequency is Fcenter * 2^(control * N)
    // where "control" ranges from -1.0 to +1.0
    // and "N" allows the frequency to change from 0 to 7 octaves
    if (n < 0.0) n = 0.0;
    else if (n > 6.9999) n = 6.9999;
    setting_octavemult = n * 4096.0;
  }


  using AudioStream::release;
  virtual void update(void);

private:
  audio_block_t *inputQueueArray[2];

  int32_t frequency;
  int16_t resonance;
  int32_t setting_octavemult; // q3.12 - 7 octaves max modulation

  int32_t b_aflt1, b_aflt2, b_aflt3, b_aflt4;

};

#endif


