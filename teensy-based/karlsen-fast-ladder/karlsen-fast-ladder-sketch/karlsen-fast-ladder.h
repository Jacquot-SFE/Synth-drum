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
  AudioFilterKarlsen() : AudioStream(1, inputQueueArray) 
  {
    b_aflt1 = b_aflt2 = b_aflt3 = b_aflt4 = 0;
    
    //frequency = 0x1000;
    //frequency = 0x0800;
    resonance = 0;
  };

  void cutoff(int16_t val)
  {
    frequency = val;
  };
  void q(int16_t val)
  {
    resonance = val;
  };

  using AudioStream::release;
  virtual void update(void);

private:
  audio_block_t *inputQueueArray[1];

  int16_t frequency;
  int16_t resonance;

  int32_t b_aflt1, b_aflt2, b_aflt3, b_aflt4;

};

#endif


